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

#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>
#include <QDebug>
#include <QFile>

#include <cstdlib>

#include "widget.h"
#include "ui_widget.h"

#include "networkaccess.h"
#include "csvreader.h"

static const char * const TICKERS_DB = "tickers.sqlite";
static const char * const TICKERS_DB_NEW = "tickers.sqlite.new";
static const char * const TICKERS_DB_TS = "tickers.sqlite.timestamp";

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "old");
        db.setDatabaseName(TICKERS_DB);

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

    QFile::remove(TICKERS_DB_NEW);
    QFile::remove(TICKERS_DB_TS);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(TICKERS_DB_NEW);

    if(!db.open())
    {
        qDebug("Cannot open database (%s)", qPrintable(db.lastError().text()));
        QMessageBox::critical(0, "Fatal error", QString("Cannot open database (%1)").arg(db.lastError().text()));
        ::exit(1);
    }

    m_net = new NetworkAccess(this);

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));

    QTimer::singleShot(0, this, SLOT(slotGet()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slotGet()
{
    ui->plainTextLog->clear();

    QSqlQuery query;
    int ok = 0;

    ok += query.exec("CREATE TABLE IF NOT EXISTS tickers ("
                         "ticker VARCHAR(10) UNIQUE,"
                         "company VARCHAR(128),"
                         "sector VARCHAR(128),"
                         "industry VARCHAR(128)"
                         ");");

    ok += query.exec("DELETE FROM tickers");

    if(ok != 2)
    {
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
        qDebug("Network error #%d", m_net->error());
        ui->plainTextLog->appendPlainText(QString("Network error #%1").arg(m_net->error()));
        QSqlDatabase::database().commit();
        return;
    }

    QByteArray ba = m_net->data();
    CsvParser csv(ba);
    QStringList str;
    csv.parseLine();
    int num = 0;
    QStringList newTickers;

    while(!(str = csv.parseLine()).isEmpty())
    {
        if(str.size() < 5)
        {
            ui->plainTextLog->appendPlainText("Broken data");
            return;
        }

        newTickers.append(str[1]);

        if(!writeData(str[1], str[2], str[3], str[4]))
            return;

        ui->label->setNum(++num);

        if(!(num % 100))
            qApp->processEvents();
    }

    QSqlDatabase::database().commit();
    QSqlDatabase::database().close();

    newTickers.sort();

    if(newTickers != oldTickers)
    {
        qDebug("Need update");
        ui->plainTextLog->appendPlainText("Need update");

        if(!QFile::remove(TICKERS_DB) || !QFile::copy(TICKERS_DB_NEW, TICKERS_DB))
        {
            qDebug("Cannot copy");
            ui->plainTextLog->appendPlainText("Cannot copy");
        }
        else
        {
            QFile::remove(TICKERS_DB_NEW);

            QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
            QFile fts(TICKERS_DB_TS);

            if(!fts.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            {
                qDebug("Cannot open timestamp file");
                ui->plainTextLog->appendPlainText(QString("Cannot open timestamp file (%1)").arg(fts.errorString()));
            }
            else
            {
                fts.write(ts.toAscii());
                fts.close();

                qDebug("Update done %s", qPrintable(ts));
                ui->plainTextLog->appendPlainText(QString("Done %1").arg(ts));
            }
        }
    }
    else
    {
        qDebug("Up-to-date");
        QFile::remove(TICKERS_DB_NEW);
        ui->plainTextLog->appendPlainText("Up-to-date");
    }
}

bool Widget::writeData(const QString &ticker, const QString &company, const QString &sector, const QString &industry)
{
    QSqlQuery query;

    query.prepare("INSERT INTO tickers (ticker, company, sector, industry) "
                  "VALUES (:ticker, :company, :sector, :industry)");

    query.bindValue(":ticker", ticker);
    query.bindValue(":company", company);
    query.bindValue(":sector", sector);
    query.bindValue(":undustry", industry);

    if(!query.exec())
    {
        qDebug("Cannot query (%s)", qPrintable(db.lastError().text()));
        ui->plainTextLog->appendPlainText(QString("Cannot query (%1)").arg(qPrintable(db.lastError().text())));
        return false;
    }

    ui->plainTextLog->appendPlainText(ticker);

    return true;
}
