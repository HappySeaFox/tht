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

#include <QByteArray>
#include <QTimer>
#include <QFile>
#include <QUrl>

#include "tickersdatabaseupdater.h"
#include "networkaccess.h"
#include "settings.h"

TickersDatabaseUpdater::TickersDatabaseUpdater(QObject *parent) :
    QObject(parent),
    m_downloadingData(false)
{
    checkNewData();

    // reread database timestamps
    Settings::instance()->rereadTimestamps();

    m_baseurl = SVNROOT "/trunk/tickersdb/";

    m_net = new NetworkAccess(this);

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));

    if(Settings::instance()->persistentDatabaseTimestamp().isValid())
        startRequest();
    else
        qDebug("Persistent database timestamp is invalid");
}

void TickersDatabaseUpdater::startRequest()
{
    m_net->get(QUrl(m_baseurl + (m_downloadingData ? "tickers.sqlite" : "tickers.sqlite.timestamp")));
}

void TickersDatabaseUpdater::checkNewData()
{
    QString oldDb = Settings::instance()->mutableDatabasePath();
    QString newDb = oldDb + ".new";

    QString oldTs = oldDb + ".timestamp";
    QString newTs = oldTs + ".new";

    if(!QFile::exists(newDb) || !QFile::exists(newTs))
    {
        QFile::remove(newDb);
        QFile::remove(newTs);

        qDebug("No new database found locally");
        return;
    }

    qDebug("Copying new database");

    QFile::remove(oldDb);
    QFile::remove(oldTs);

    if(!QFile::copy(newDb, oldDb))
        qDebug("Cannot copy new database");

    if(!QFile::copy(newTs, oldTs))
        qDebug("Cannot copy new timestamp");

    QFile::remove(newDb);
    QFile::remove(newTs);
}

bool TickersDatabaseUpdater::writeData(const QString &fileName, const QByteArray &data)
{
    QFile file(fileName + ".new");

    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug("Cannot open file for writing (%s)", qPrintable(file.errorString()));
        file.remove();
        return false;
    }

    qint64 written = file.write(data);

    if(written != data.size())
    {
        qDebug("Not all data written (%ld written, %ld required)", (long)written, (long)data.size());
        file.remove();
        return false;
    }

    return true;
}

void TickersDatabaseUpdater::slotFinished()
{
    if(m_net->error() != QNetworkReply::NoError)
    {
        m_net->clearBuffer();

        // recheck in 1 hour
        QTimer::singleShot(1*3600*1000, this, SLOT(startRequest()));
        return;
    }

    if(m_downloadingData)
    {
        writeData(Settings::instance()->mutableDatabasePath(), m_net->data());
        m_net->clearBuffer();
    }
    else
    {
        QDateTime ts = QDateTime::fromString(m_net->data().trimmed(), Settings::instance()->databaseTimestampFormat());

        if(!ts.isValid())
        {
            qDebug("New database timestamp is invalid");
            return;
        }

        if(ts <= Settings::instance()->persistentDatabaseTimestamp()
                || (Settings::instance()->mutableDatabaseTimestamp().isValid()
                    && ts <= Settings::instance()->mutableDatabaseTimestamp()))
        {
            qDebug("No database updates available");
            QTimer::singleShot(1*3600*1000, this, SLOT(startRequest()));
            return;
        }

        if(!writeData(Settings::instance()->mutableDatabasePath() + ".timestamp", m_net->data()))
            return;

        qDebug("Downloading new database");

        m_downloadingData = true;

        QTimer::singleShot(0, this, SLOT(startRequest()));
    }
}
