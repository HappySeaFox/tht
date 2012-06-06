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

    m_baseurl = "http://traders-home-task-ng.googlecode.com/svn/trunk/tickersdb/";

    QFile file(Settings::instance()->tickersPersistentDatabasePath() + ".timestamp");

    if(file.open(QIODevice::ReadOnly))
        m_timestamp = QDateTime::fromString(file.readAll().trimmed(), "yyyy-MM-dd hh:mm:ss.zzz");

    file.close();

    m_net = new NetworkAccess(this);

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));

    if(m_timestamp.isValid())
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
    QString oldDb = Settings::instance()->tickersMutableDatabasePath();
    QString newDb = oldDb + ".new";

    QString oldTs = oldDb + ".timestamp";
    QString newTs = oldTs + ".new";

    if(!QFile::exists(newDb) || !QFile::exists(newTs))
    {
        qDebug("No new database found locally");
        return;
    }

    qDebug("Copying new database");

    QFile::remove(oldDb);
    QFile::copy(newDb, oldDb);
    QFile::remove(newDb);

    QFile::remove(oldTs);
    QFile::copy(newTs, oldTs);
    QFile::remove(newTs);
}

bool TickersDatabaseUpdater::writeData(const QString &fileName, const QByteArray &data)
{
    QFile file(fileName + ".new");

    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug("Cannot open file for writing (%s)", qPrintable(file.errorString()));
        return false;
    }

    qint64 written = file.write(data);

    if(written != data.size())
    {
        file.close();
        QFile::remove(file.fileName());
        return false;
    }

    return true;
}

void TickersDatabaseUpdater::slotFinished()
{
    if(m_net->error() != QNetworkReply::NoError)
    {
        // recheck in 1 hour
        QTimer::singleShot(1*3600*1000, this, SLOT(startRequest()));
        return;
    }

    if(m_downloadingData)
    {
        if(!writeData(Settings::instance()->tickersMutableDatabasePath(), m_net->data()))
            QFile::remove(Settings::instance()->tickersMutableDatabasePath() + ".timestamp.new");

        m_net->clearBuffer();
    }
    else
    {
        QDateTime ts = QDateTime::fromString(m_net->data().trimmed(), "yyyy-MM-dd hh:mm:ss.zzz");

        if(!ts.isValid())
        {
            qDebug("New database timestamp is invalid");
            return;
        }

        if(m_timestamp != ts)
        {
            qDebug("No database updates available");
            return;
        }

        if(!writeData(Settings::instance()->tickersMutableDatabasePath() + ".timestamp", m_net->data()))
            return;

        qDebug("Downloading new database");

        m_downloadingData = true;

        QTimer::singleShot(0, this, SLOT(startRequest()));
    }
}
