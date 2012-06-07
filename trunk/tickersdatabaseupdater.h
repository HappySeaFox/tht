#ifndef TICKERSDATABASEUPDATER_H
#define TICKERSDATABASEUPDATER_H

#include <QDateTime>
#include <QObject>
#include <QString>

class QByteArray;
class NetworkAccess;

class TickersDatabaseUpdater : public QObject
{
    Q_OBJECT

public:
    explicit TickersDatabaseUpdater(QObject *parent = 0);

private:
    void checkNewData();
    bool writeData(const QString &file, const QByteArray &data);
    QDateTime readTimestamp(const QString &fileName) const;

private slots:
    void startRequest();
    void slotFinished();

private:
    NetworkAccess *m_net;
    QString m_baseurl;
    QDateTime m_timestampP, m_timestampM;
    bool m_downloadingData;
};

#endif // TICKERSDATABASEUPDATER_H
