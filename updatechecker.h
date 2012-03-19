#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QSslError>
#include <QObject>
#include <QRegExp>
#include <QList>
#include <QUrl>

class QNetworkAccessManager;
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
    void slotFinished(QNetworkReply *);
    void slotSslErrors(QNetworkReply*, const QList<QSslError> &list);

private:
    explicit UpdateChecker();

    QNetworkAccessManager *m_net;
    QString m_lastVersion;
    QRegExp m_rxVersion;
    QUrl m_url;
};

#endif // UPDATECHECKER_H
