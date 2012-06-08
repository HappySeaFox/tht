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

    QSqlDatabase::removeDatabase("old");

    oldTickers.sort();

    qDebug("Loaded %d old values", oldTickers.size());

    QFile::remove(THT_TICKERS_DB_NEW);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(THT_TICKERS_DB_NEW);

    if(!db.open())
    {
        qDebug("Cannot open database (%s)", qPrintable(db.lastError().text()));
        QMessageBox::critical(0, "Fatal error", QString("Cannot open database (%1)").arg(db.lastError().text()));
        ::exit(1);
    }

    m_net = new NetworkAccess(this);

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent *e)
{
    if(!m_running || QMessageBox::question(this, "Quit", "Really quit?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        exit(0);
    else
        e->ignore();
}

void Widget::slotGet()
{
    m_running = true;
    ui->checkForce->setEnabled(false);
    ui->pushGet->setEnabled(false);
    ui->plainTextLog->clear();
    qApp->processEvents();

    QSqlQuery query;
    int ok = 0;

    ok += query.exec("CREATE TABLE IF NOT EXISTS tickers ("
                         "ticker VARCHAR(10) UNIQUE,"
                         "company VARCHAR(64),"
                         "sector VARCHAR(64),"
                         "industry VARCHAR(64),"
                         "exchange VARCHAR(16),"
                         "cap DOUBLE"
                         ");");

    ok += query.exec("DELETE FROM tickers");

    if(ok != 2)
    {
        m_running = false;
        qDebug("Cannot query (%s)", qPrintable(db.lastError().text()));
        ui->plainTextLog->appendPlainText(QString("Cannot query (%1)").arg(qPrintable(db.lastError().text())));
        return;
    }

    QSqlDatabase::database().transaction();

    m_net->get(QUrl("http://finviz.com/export.ashx?v=150&o=ticker"));
}

void Widget::slotFinished()
{
    if(m_net->error() != QNetworkReply::NoError)
    {
        error(QString("Network error #%1").arg(m_net->error()));
        QSqlDatabase::database().commit();
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
            error(QString("Broken data (%1 fields)").arg(str.size()));
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

        ui->label->setNum(++num);
    }

    newTickers.sort();

    if(newTickers == oldTickers && !ui->checkForce->isChecked())
    {
        QFile::remove(THT_TICKERS_DB_NEW);
        error("Up-to-date, will quit in 5 sec");
        QTimer::singleShot(5000, this, SLOT(close()));
        QApplication::alert(this);
        return;
    }

    disconnect(m_net, SIGNAL(finished()), this, 0);
    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinishedExchange()));

    num = 0;
    QMap<QString, Ticker>::iterator itEnd = map.end();

    for(QMap<QString, Ticker>::iterator it = map.begin();it != itEnd;++it)
    {
        QEventLoop eventLoop;
        connect(m_net, SIGNAL(finished()), &eventLoop, SLOT(quit()));

        m_net->get(QUrl(QString("http://finviz.com/quote.ashx?t=%1&ty=l&ta=0&p=d").arg(it.key())));

        exchange.clear();
        eventLoop.exec();

        if(exchange.isEmpty())
        {
            ui->plainTextLog->appendPlainText(QString("Cannot fetch exchange name for %1").arg(it.key()));
            --it;
            continue;
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

    QSqlDatabase::database().commit();
    QSqlDatabase::database().close();

    qDebug("Need update");
    ui->plainTextLog->appendPlainText("Need update");

    if(!QFile::remove(THT_TICKERS_DB) || !QFile::copy(THT_TICKERS_DB_NEW, THT_TICKERS_DB))
    {
        error("Cannot copy");
        return;
    }

    QFile::remove(THT_TICKERS_DB_NEW);

    m_ts = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QFile fts(THT_TICKERS_DB_TS);

    if(!fts.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        error(QString("Cannot open timestamp file (%1)").arg(fts.errorString()));
        return;
    }

    if(fts.write(m_ts.toAscii()) != m_ts.length())
    {
        error(QString("Cannot write all the data (%1)").arg(fts.errorString()));
        return;
    }

    fts.close();

    qDebug("Update done %s", qPrintable(m_ts));
    ui->plainTextLog->appendPlainText(QString("Done %1").arg(m_ts));
    ui->pushCommit->setEnabled(true);

    m_running = false;
}

void Widget::slotCommit()
{
    if(m_ts.isEmpty())
        return;

    ui->pushCommit->setEnabled(false);

    QProcess p;

    p.start("svn", QStringList()
                    << "-m"
                    << ("Database update " + m_ts)
                    << "commit"
                    << THT_TICKERS_DB
                    << THT_TICKERS_DB_TS);

    if(!p.waitForFinished())
    {
        error(QString("Commit failed due to process timeout (%1)").arg(p.error()));
        return;
    }

    ui->plainTextLog->appendPlainText(p.readAll());

    int code = p.exitCode() || (p.exitStatus() != QProcess::NormalExit);

    if(code)
    {
        error(QString("Commit failed (%1)").arg(code));
        return;
    }

    qDebug("Commit done");
    ui->plainTextLog->appendPlainText("Commit done");
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
        error(QString("Cannot query (%1)").arg(qPrintable(db.lastError().text())));
        return false;
    }

    ui->plainTextLog->appendPlainText(t.ticker + '/' + t.exchange + '/' + QString::number(t.cap, 'f'));

    return true;
}

void Widget::error(const QString &e)
{
    qDebug("%s", qPrintable(e));

    m_running = false;
    ui->plainTextLog->appendPlainText(e);
    QApplication::alert(this);
}
