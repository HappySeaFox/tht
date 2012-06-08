#include <QByteArray>
#include <QTimer>
#include <QFile>
#include <QUrl>

#include "tickersdatabaseupdater.h"
#include "networkaccess.h"
#include "settings.h"

static const char * const THT_TIMESTAMP_FORMAT = "yyyy-MM-dd hh:mm:ss.zzz";

TickersDatabaseUpdater::TickersDatabaseUpdater(QObject *parent) :
    QObject(parent),
    m_downloadingData(false)
{
    checkNewData();

    m_baseurl = "http://traders-home-task-ng.googlecode.com/svn/trunk/tickersdb/";

    m_timestampP = readTimestamp(Settings::instance()->tickersPersistentDatabasePath());
    m_timestampM = readTimestamp(Settings::instance()->tickersMutableDatabasePath());

    qDebug("Database P timestamp: %s", qPrintable(m_timestampP.toString(THT_TIMESTAMP_FORMAT)));
    qDebug("Database M timestamp: %s", qPrintable(m_timestampM.toString(THT_TIMESTAMP_FORMAT)));

    m_net = new NetworkAccess(this);

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));

    if(m_timestampP.isValid())
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

QDateTime TickersDatabaseUpdater::readTimestamp(const QString &fileName) const
{
    QFile file(fileName + ".timestamp");

    if(file.open(QIODevice::ReadOnly))
        return QDateTime::fromString(file.readAll().trimmed(), THT_TIMESTAMP_FORMAT);

    return QDateTime();
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
        writeData(Settings::instance()->tickersMutableDatabasePath(), m_net->data());
        m_net->clearBuffer();
    }
    else
    {
        QDateTime ts = QDateTime::fromString(m_net->data().trimmed(), THT_TIMESTAMP_FORMAT);

        if(!ts.isValid())
        {
            qDebug("New database timestamp is invalid");
            return;
        }

        if(ts <= m_timestampP && (!m_timestampM.isValid() || ts <= m_timestampM))
        {
            qDebug("No database updates available");
            QTimer::singleShot(1*3600*1000, this, SLOT(startRequest()));
            return;
        }

        if(!writeData(Settings::instance()->tickersMutableDatabasePath() + ".timestamp", m_net->data()))
            return;

        qDebug("Downloading new database");

        m_downloadingData = true;

        QTimer::singleShot(0, this, SLOT(startRequest()));
    }
}
