#ifndef TICKERINFORMATIONFETCHER_H
#define TICKERINFORMATIONFETCHER_H

#include <QObject>
#include <QString>

class QTimer;

class TickerInformationFetcher : public QObject
{
    Q_OBJECT

public:
    explicit TickerInformationFetcher(QObject *parent = 0);

    void fetch(const QString &);

signals:
    // name is empty on error
    void done(const QString &name = QString(),
              const QString &sector = QString(), const QString &industry = QString());

private slots:
    void slotFetch();

private:
    QString m_ticker;
    QTimer *m_timer;
};

#endif // TICKERINFORMATIONFETCHER_H
