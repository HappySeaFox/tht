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

#include <QWebElementCollection>
#include <QWebSettings>
#include <QApplication>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QWebElement>
#include <QCloseEvent>
#include <QEventLoop>
#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QWebFrame>
#include <QProcess>
#include <QWebPage>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QMap>

#include <cstdlib>
#include <cmath>

#include "widget.h"
#include "ui_widget.h"

#include "networkaccess.h"
#include "csvreader.h"

static const char * const THT_TICKERS_DB     = "tickers.sqlite";
static const char * const THT_TICKERS_DB_NEW = "tickers.sqlite.new";
static const char * const THT_TICKERS_DB_TS  = "tickers.sqlite.timestamp";

Widget::Widget(QWidget *parent) :
    QWidget(parent,
            Qt::Window
            | Qt::WindowMinimizeButtonHint
            | Qt::WindowCloseButtonHint
            | Qt::CustomizeWindowHint),
    ui(new Ui::Widget),
    m_running(false)
{
    ui->setupUi(this);

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "old");
        db.setDatabaseName(THT_TICKERS_DB);

        if(!db.open())
        {
            qDebug("Cannot open database (%s)", qPrintable(db.lastError().text()));
            QMessageBox::critical(0, "Fatal error", QString("Cannot open database (%1)").arg(db.lastError().text()));
            ::exit(1);
        }

        QSqlQuery query("SELECT * FROM tickers", db);

        while(query.next())
            oldTickers.append(query.value(0).toString());
    }

    oldTickers.sort();

    qDebug("Loaded %d old values", oldTickers.size());

    QFile::remove(THT_TICKERS_DB_NEW);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(THT_TICKERS_DB_NEW);

    if(!db.open())
    {
        qDebug("Cannot open database (%s)", qPrintable(db.lastError().text()));
        QMessageBox::critical(0, "Fatal error", QString("Cannot open database (%1)").arg(db.lastError().text()));
        ::exit(1);
    }

    m_net = new NetworkAccess(this);

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));

    m_auto = QApplication::arguments().indexOf("auto") >= 0;

    if(m_auto)
        QTimer::singleShot(0, this, SLOT(slotGet()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent *e)
{
    e->ignore();

    if(!m_running || QMessageBox::question(this, "Quit", "Really quit?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        qApp->quit();
}

void Widget::slotGet()
{
    m_running = true;
    ui->checkForce->setEnabled(false);
    ui->pushGet->setEnabled(false);
    ui->plainTextLog->clear();
    qApp->processEvents();

    QSqlQuery query;

    if(!query.exec("CREATE TABLE tickers ("
                         "ticker VARCHAR(16) UNIQUE,"
                         "company VARCHAR(128),"
                         "sector VARCHAR(64),"
                         "industry VARCHAR(64),"
                         "exchange VARCHAR(16),"
                         "cap DOUBLE"
                         ");"))
    {
        message(QString("Cannot query (%1)").arg(qPrintable(QSqlDatabase::database().lastError().text())));
        return;
    }

    m_net->get(QUrl("http://finviz.com/export.ashx?v=150&o=ticker"));
}

void Widget::slotFinished()
{
    if(m_net->error() != QNetworkReply::NoError)
    {
        message(QString("Network error #%1").arg(m_net->error()));
        return;
    }

    QByteArray ba = m_net->data();
    CsvParser csv(ba);
    QStringList str;
    csv.parseLine();
    int num = 0;
    QStringList newTickers;
    QMap<QString, Ticker> map;
    Ticker t;
    bool ok;

    while(!(str = csv.parseLine()).isEmpty())
    {
        if(str.size() != 11)
        {
            message(QString("Broken data (%1 fields)").arg(str.size()));
            return;
        }

        t.ticker = str[1];
        t.company = str[2];
        t.sector = str[3];
        t.industry = str[4];
        t.cap = str[6].toDouble(&ok);

        if(!ok)
            t.cap = 0;

        newTickers.append(str[1]);

        map.insert(str[1], t);

    }

    ui->label->setNum(newTickers.size());

    bool needup = false;
    newTickers.sort();
    bool newSymbols = newTickers != oldTickers;

    // compare capitalizations
    if(!ui->checkForce->isChecked() && !newSymbols)
    {
        qDebug("Checking capitalizations");

        foreach(QString ticker, newTickers)
        {
            QSqlQuery query(QSqlDatabase::database("old"));

            query.prepare("SELECT cap FROM tickers WHERE ticker = :ticker");
            query.bindValue(":ticker", ticker);

            if(!query.exec() || !query.next())
            {
                message(QString("Cannot query (%1)").arg(qPrintable(QSqlDatabase::database().lastError().text())));
                return;
            }

            bool ok;
            double cap = query.value(0).toDouble(&ok);

            if(!ok)
            {
                message("Double value is broken");
                return;
            }

            double oldCap = map[ticker].cap;

            if(!oldCap && !cap)
            {
                qDebug("Both capitalizations are 0 for %s", qPrintable(ticker));
                continue;
            }

            if((cap && !oldCap) || fabs((oldCap - cap) / oldCap) > 5) // more than 5%
            {
                qDebug("Capitalization for %s is changed too much (%.1f / %.1f)", qPrintable(ticker), cap, oldCap);
                needup = true;
                break;
            }
        }
    }
    else
        needup = true;

    // up-to-date
    if(!needup)
    {
        QFile::remove(THT_TICKERS_DB_NEW);
        message("Up-to-date", false);

        if(m_auto)
            QTimer::singleShot(5000, this, SLOT(close()));

        return;
    }

    if(newSymbols)
    {
        qDebug("Difference by symbol (%d/%d):", oldTickers.size(), newTickers.size());

        QSet<QString> oldSet = oldTickers.toSet();
        QSet<QString> newSet = newTickers.toSet();

        QSet<QString> minus = oldSet - newSet;
        QSet<QString> plus = newSet - oldSet;

        foreach(QString s, minus)
        {
            qDebug("-%s", qPrintable(s));
            ui->plainTextLog->appendPlainText('-' + s);
        }

        foreach(QString s, plus)
        {
            qDebug("+%s", qPrintable(s));
            ui->plainTextLog->appendPlainText('+' + s);
        }
    }

    disconnect(m_net, SIGNAL(finished()), this, 0);
    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinishedExchange()));

    QSqlDatabase::database().transaction();

    QMap<QString, Ticker>::iterator itEnd = map.end();

    for(QMap<QString, Ticker>::iterator it = map.begin();it != itEnd;++it)
    {
        QEventLoop eventLoop;
        connect(m_net, SIGNAL(finished()), &eventLoop, SLOT(quit()));

        m_net->get(QUrl(QString("http://finviz.com/quote.ashx?t=%1&ty=l&ta=0&p=d").arg(it.key())));

        exchange.clear();
        eventLoop.exec();

        qApp->processEvents();

        if(m_net->error() != QNetworkReply::NoError)
        {
            message(QString("Network error #%1").arg(m_net->error()));
            return;
        }

        if(exchange.isEmpty())
        {
            message(QString("Cannot fetch exchange name for %1").arg(it.key()));
            return;
        }

        ui->labelProgress->setNum(++num);

        t = it.value();

        t.exchange = exchange;

        if(!writeData(t))
        {
            m_running = false;
            return;
        }
    }

    QSqlDatabase::removeDatabase("old");

    QSqlDatabase::database().commit();
    QSqlDatabase::database().close();

    qDebug("Need update");
    ui->plainTextLog->appendPlainText("Need update");

    if(!QFile::remove(THT_TICKERS_DB))
    {
        message("Cannot remove old database");
        return;
    }

    if(!QFile::copy(THT_TICKERS_DB_NEW, THT_TICKERS_DB))
    {
        message("Cannot copy");
        return;
    }

    QFile::remove(THT_TICKERS_DB_NEW);

    m_ts = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QFile fts(THT_TICKERS_DB_TS);

    if(!fts.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        message(QString("Cannot open timestamp file (%1)").arg(fts.errorString()));
        return;
    }

    if(fts.write(m_ts.toAscii()) != m_ts.length())
    {
        message(QString("Cannot write all the data (%1)").arg(fts.errorString()));
        return;
    }

    fts.close();

    ui->pushCommit->setEnabled(true);

    if(m_auto && commit())
    {
        message(QString("Done update to %1, will quit in 5 sec").arg(m_ts));
        QTimer::singleShot(5000, this, SLOT(close()));
    }

    m_running = false;
}

bool Widget::commit()
{
    if(m_ts.isEmpty())
    {
        qDebug("Timestamp is empty");
        return false;
    }

    QProcess p;

    p.start("svn", QStringList()
                    << "-m"
                    << ("Database update " + m_ts)
                    << "commit"
                    << THT_TICKERS_DB
                    << THT_TICKERS_DB_TS);

    if(!p.waitForStarted())
    {
        message(QString("Commit failed due to process error (%1)").arg(p.error()));
        return false;
    }

    if(!p.waitForFinished())
    {
        message(QString("Commit failed due to process timeout (%1)").arg(p.error()));
        return false;
    }

    ui->plainTextLog->appendPlainText(p.readAll());

    int code = p.exitCode() || (p.exitStatus() != QProcess::NormalExit);

    if(code)
    {
        message(QString("Commit failed (%1)").arg(code));
        return false;
    }

    qDebug("Commit done");
    ui->plainTextLog->appendPlainText("Commit done");

    return true;
}

void Widget::slotFinishedExchange()
{
    if(m_net->error() != QNetworkReply::NoError)
        return;

    QRegExp rx("^\\[(.*)\\]$");
    QWebPage page;

    page.settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    page.settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    page.settings()->setAttribute(QWebSettings::JavaEnabled, false);
    page.settings()->setAttribute(QWebSettings::PluginsEnabled, false);

    page.mainFrame()->setHtml(m_net->data());

    // ticker name
    QWebElement table = page.mainFrame()->findFirstElement("table.fullview-title");

    QString s = table.findFirst("tr").findFirst("td").findFirst("a.fullview-ticker").nextSibling().toPlainText();

    if(rx.exactMatch(s))
        exchange = rx.cap(1);
}

bool Widget::writeData(const Ticker &t)
{
    QSqlQuery query;

    query.prepare("INSERT INTO tickers (ticker, company, sector, industry, exchange, cap) "
                  "VALUES (:ticker, :company, :sector, :industry, :exchange, :cap)");

    query.bindValue(":ticker", t.ticker);
    query.bindValue(":company", t.company);
    query.bindValue(":sector", t.sector);
    query.bindValue(":industry", t.industry);
    query.bindValue(":exchange", t.exchange);
    query.bindValue(":cap", t.cap);

    if(!query.exec())
    {
        message(QString("Cannot query (%1)").arg(qPrintable(QSqlDatabase::database().lastError().text())));
        return false;
    }

    ui->plainTextLog->appendPlainText(t.ticker + '/' + t.exchange + '/' + QString::number(t.cap, 'f', 2));

    return true;
}

void Widget::message(const QString &e, bool activate)
{
    qDebug("%s", qPrintable(e));

    m_running = false;
    ui->plainTextLog->appendPlainText(e);

    if(activate && m_auto)
    {
        show();
        setWindowState(windowState() & ~Qt::WindowMinimized);
        raise();
        activateWindow();
    }

    if(activate)
        QApplication::alert(this);
}
