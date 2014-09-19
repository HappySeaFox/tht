/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QNetworkRequest>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QUrl>

#include "networkaccess.h"
#include "csvreader.h"
#include "sqltools.h"
#include "settings.h"

#include "tickersdatabaseupdater.h"
#include "idletimer.h"

struct TickerForDb
{
    TickerForDb()
        : cap(0)
    {}

    QString ticker;
    QString company;
    QString sector;
    QString industry;
    QString exchange;
    QString country;

    double cap;
};

inline bool operator<(const TickerForDb &a, const TickerForDb &b)
{
    return a.ticker < b.ticker;
}

/*******************************************/

TickersDatabaseUpdater::TickersDatabaseUpdater(QObject *parent) :
    QObject(parent)
{
    if(!Settings::instance()->persistentDatabaseTimestamp().isValid())
    {
        qDebug("Persistent database timestamp is invalid");
        return;
    }

    // don't update db more than once in 24 hours
    if(Settings::instance()->mutableDatabaseTimestamp().isValid()
            && (QDateTime::currentMSecsSinceEpoch()
                - Settings::instance()->mutableDatabaseTimestamp().toMSecsSinceEpoch()) < 24*3600*1000)
    {
        qDebug("Won't update tickers database right now");
        return;
    }

    m_newDb = Settings::instance()->mutableDatabasePath() + ".new";
    m_newTs = Settings::instance()->mutableDatabasePath() + ".timestamp.new";

    if(QFile::exists(m_newDb) && !QFile::remove(m_newDb))
    {
        qWarning("Cannot remove database template");
        return;
    }

    if(QFile::exists(m_newTs) && !QFile::remove(m_newTs))
    {
        qWarning("Cannot remove timestamp template");
        return;
    }

    m_exchanges = QStringList() << "NYSE" << "NASD" << "AMEX";

    m_net = new NetworkAccess(this);

    m_idleTimer = new IdleTimer(this);
    m_idleTimer->setEnabled(true);
    connect(m_idleTimer, SIGNAL(idle()), this, SLOT(slotStartRequest()));
}

void TickersDatabaseUpdater::slotFinishedFomc()
{
    if(m_net->error() != QNetworkReply::NoError)
        return;

    QString data = m_net->data();

    if(!addFomcDates(data, "FOMC Meeting Announcement (US)")
            || !addFomcDates(data, "FOMC Minutes (US)"))
    {
        qWarning("FOMC parsing error");
    }

    proceedToTickers();
}

void TickersDatabaseUpdater::slotFinished()
{
    if(m_net->error() != QNetworkReply::NoError)
        return;

    m_data.insert(m_exchanges.takeFirst(), m_net->data());

    if(!nextExchange())
    {
        m_idleTimer->setInterval(7000);
        m_idleTimer->setEnabled(true);

        disconnect(m_idleTimer, SIGNAL(idle()), this, 0);
        connect(m_idleTimer, SIGNAL(idle()), this, SLOT(slotParseAnswer()));
    }
}

void TickersDatabaseUpdater::slotStartRequest()
{
    qDebug("Starting database update request");

    m_idleTimer->setEnabled(false);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "generateddb");
    db.setDatabaseName(m_newDb);

    if(!db.open())
    {
        qWarning("Cannot open database: %s", qPrintable(db.lastError().text()));
        return;
    }

    // FOMC dates
    QList<QVariantList> result = SqlTools::query("SELECT date FROM fomc");

    QString line;
    QDate date;

    foreach(QVariantList vl, result)
    {
        if(vl.size() != 1)
        {
            qWarning("FOMC result list must have exactly only one record (records: %d)", vl.size());
            m_fomcDates.clear();
            break;
        }

        line = vl.at(0).toString();
        date = QDate::fromString(line, "yyyy MM dd");

        if(!date.isValid())
        {
            qWarning("FOMC date \"%s\" is invalid", qPrintable(line));
            m_fomcDates.clear();
            break;
        }

        qDebug("Added FOMC date %s", qPrintable(line));
        m_fomcDates.append(date);
    }

    // check for correct year
    int fomcYear = m_fomcDates.isEmpty() ? -1 : m_fomcDates.first().year();
    int year = QDate::currentDate().year();

    if(fomcYear != year)
    {
        m_fomcDates.clear();
        connect(m_net, SIGNAL(finished()), this, SLOT(slotFinishedFomc()));
        m_net->startRequest(QNetworkAccessManager::GetOperation,
                            QNetworkRequest(QUrl(QString("http://mam.econoday.com/release_dates.asp?cust=mam&year=%1").arg(year))));
    }
    else
    {
        qDebug("No need to update FOMC");
        proceedToTickers();
    }
}

void TickersDatabaseUpdater::slotParseAnswer()
{
    if(m_data.isEmpty())
        return;

    qDebug("Saving the new database");

    qint64 v = QDateTime::currentMSecsSinceEpoch();

    QList<TickerForDb> tickers;

    tickers.reserve(7000);

    QMap<QString, QByteArray>::iterator itdEnd = m_data.end();

    for(QMap<QString, QByteArray>::iterator it = m_data.begin();it != itdEnd;++it)
    {
        CsvReader csv(it.value());

        QStringList str;
        TickerForDb t;
        bool ok;

        while(!(str = csv.parseLine()).isEmpty())
        {
            if(str.size() != 11)
            {
                qWarning("Broken data (%d fields)", str.size());
                m_data.clear();
                return;
            }

            if(str[0] == "No.")
                continue;

            t.ticker = str[1];
            t.company = str[2];
            t.sector = str[3];
            t.industry = str[4];
            t.exchange = it.key();
            t.country = str[5];
            t.cap = str[6].toDouble(&ok);

            if(!ok)
                t.cap = 0;

            tickers.append(t);
        }
    }

    qSort(tickers);

    m_data.clear();

    QSqlDatabase::database("generateddb").transaction();

    QSqlQuery query(QSqlDatabase::database("generateddb"));

    if(!query.exec("CREATE TABLE tickers ("
                   "ticker VARCHAR(16) UNIQUE,"
                   "company VARCHAR(128),"
                   "sector VARCHAR(64),"
                   "industry VARCHAR(64),"
                   "exchange VARCHAR(16),"
                   "country VARCHAR(64),"
                   "cap DOUBLE"
                   ");") ||
            !query.exec("CREATE TABLE fomc ("
                        "date VARCHAR(10)"
                        ");"))
    {
        qWarning("Cannot create table: %s", qPrintable(query.lastError().text()));
        return;
    }

    if(!query.prepare("INSERT INTO tickers (ticker, company, sector, industry, exchange, country, cap) "
                  "VALUES (:ticker, :company, :sector, :industry, :exchange, :country, :cap)"))
    {
        qWarning("Cannot create table: %s", qPrintable(query.lastError().text()));
        return;
    }

    // save tickers
    foreach(const TickerForDb &t, tickers)
    {
        query.bindValue(":ticker", t.ticker);
        query.bindValue(":company", t.company);
        query.bindValue(":sector", t.sector);
        query.bindValue(":industry", t.industry);
        query.bindValue(":exchange", t.exchange);
        query.bindValue(":country", t.country);
        query.bindValue(":cap", t.cap);

        if(!query.exec())
        {
            qWarning("Cannot save ticker: %s", qPrintable(query.lastError().text()));
            return;
        }
    }

    if(!query.prepare("INSERT INTO fomc (date) VALUES (:date)"))
    {
        qWarning("Cannot create table: %s", qPrintable(query.lastError().text()));
        return;
    }

    foreach(QDate d, m_fomcDates)
    {
        query.bindValue(":date", d.toString("yyyy MM dd"));

        if(!query.exec())
        {
            qWarning("Cannot save FOMC date: %s", qPrintable(query.lastError().text()));
            return;
        }
    }

    // commit
    QSqlDatabase::database("generateddb").commit();
    QSqlDatabase::database("generateddb").close();

    QString ts = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QFile fts(m_newTs);

    if(!fts.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        qWarning("Cannot open timestamp file: %s", qPrintable(fts.errorString()));
        fts.remove();
        return;
    }

    if(fts.write(ts.toLatin1()) != ts.length())
    {
        qWarning("Cannot write all the data: %s", qPrintable(fts.errorString()));
        fts.remove();
        return;
    }

    fts.close();

    qDebug("Database has been updated in %ld ms., need restart to apply", static_cast<long int>(QDateTime::currentMSecsSinceEpoch() - v));
}

void TickersDatabaseUpdater::proceedToTickers()
{
    disconnect(m_net, SIGNAL(finished()), this, 0);
    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));

    nextExchange();
}

bool TickersDatabaseUpdater::addFomcDates(const QString &data, const QString &title)
{
    int i = data.indexOf(title);

    if(i < 0)
    {
        qWarning("FOMC title is not found");
        return false;
    }

    i = data.indexOf("Released On:", i);

    if(i < 0)
    {
        qWarning("FOMC release date is not found");
        return false;
    }

    i = data.indexOf("<td nowrap=\"nowrap\">", i);

    if(i < 0)
    {
        qWarning("FOMC HTML 'td' element is not found");
        return false;
    }

    QRegExp rx("\\s*<td\\s+nowrap=\"nowrap\"><a\\s+href=\".*\"><font\\s+face=\".*\"\\s+size=\".*\">([0-9]+/[0-9]+)</font></a></td>\\s*");

    rx.setMinimal(true);

    int year = QDate::currentDate().year();

    while(rx.indexIn(data, i) == i)
    {
        i += rx.matchedLength();

        QString cap = rx.cap(1);

        qDebug("Parsed FOMC date %s", qPrintable(cap));

        int month = cap.section('/', 0, 0).toInt();
        int day = cap.section('/', 1, 1).toInt();

        QDate date(year , month, day);

        if(!date.isValid())
            continue;

        m_fomcDates.append(date);
    }

    return true;
}

bool TickersDatabaseUpdater::nextExchange()
{
    if(m_exchanges.isEmpty())
        return false;

    qDebug("Fetching tickers for exchange %s", qPrintable(m_exchanges.at(0)));

    return m_net->startRequest(QNetworkAccessManager::GetOperation,
                                 QNetworkRequest(QUrl(QString("http://finviz.com/export.ashx?v=110&f=exch_%1")
                                                      .arg(m_exchanges.at(0).toLower()))));
}
