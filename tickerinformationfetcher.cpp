#include <QTimer>

#include "tickerinformationfetcher.h"
#include "sqltools.h"

TickerInformationFetcher::TickerInformationFetcher(QObject *parent) :
    QObject(parent)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(0);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotFetch()));
}

void TickerInformationFetcher::fetch(const QString &ticker)
{
    m_ticker = ticker;
    m_timer->start();
}

void TickerInformationFetcher::slotFetch()
{
    QList<QStringList> lists = SqlTools::query("SELECT company, sector, industry FROM tickers WHERE ticker = :ticker", ":ticker", m_ticker);

    if(!lists.isEmpty())
    {
        QStringList values = lists.at(0);

        if(values.size() > 2)
        {
            QString company = values.at(0);

            if(!company.isEmpty())
            {
                emit done(company, values.at(1), values.at(2));
                return;
            }
        }
    }

    // error
    emit done();
}
