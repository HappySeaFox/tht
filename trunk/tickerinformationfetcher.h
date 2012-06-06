#ifndef TICKERINFORMATIONFETCHER_H
#define TICKERINFORMATIONFETCHER_H

#include <QNetworkReply>
#include <QPointer>
#include <QObject>
#include <QString>

class QNetworkAccessManager;

class TickerInformationFetcher : public QObject
{
    Q_OBJECT

public:
    explicit TickerInformationFetcher(QObject *parent = 0);
    ~TickerInformationFetcher();

    void fetch(const QString &ticker);

private:
    void deleteReply();

signals:
    // name is empty on error
    void done(const QString &error, const QString &name = QString(),
              const QString &sector = QString(), const QString &industry = QString());

private slots:
    void slotNetworkError(QNetworkReply::NetworkError);
    void slotNetworkData();
    void slotNetworkDone();

private:
    QNetworkAccessManager *m_manager;
    QPointer<QNetworkReply> m_reply;
    QString m_data, m_ticker;
};

#endif // TICKERINFORMATIONFETCHER_H
