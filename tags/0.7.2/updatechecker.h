#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QSslError>
#include <QList>
#include <QRegExp>

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
};

#endif // UPDATECHECKER_H
