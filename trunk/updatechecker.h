#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QRegExp>
#include <QUrl>

class NetworkAccess;
class QNetworkReply;

class UpdateChecker : public QObject
{
    Q_OBJECT

public:
    static UpdateChecker *instance();

signals:
    void newVersion(const QString &);

private slots:
    void startRequest();
    void slotFinished();

private:
    explicit UpdateChecker();

    NetworkAccess *m_net;
    QString m_lastVersion;
    QRegExp m_rxVersion;
    QUrl m_url;
};

#endif // UPDATECHECKER_H
