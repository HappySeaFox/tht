#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QRegExp>
#include <QTimer>
#include <QUrl>

#include "tickerinformationfetcher.h"
#include "settings.h"

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
    // try to get from the database
    {
        QSqlDatabase dbp = QSqlDatabase::database(Settings::instance()->tickersPersistentDatabaseName());
        QSqlDatabase dbm = QSqlDatabase::database(Settings::instance()->tickersMutableDatabaseName());
        QString queryString = "SELECT company, sector, industry FROM tickers WHERE ticker = :ticker";

        QSqlQuery query(dbp);

        query.prepare(queryString);
        query.bindValue(":ticker", m_ticker);
        query.exec();

        if(!query.next())
        {
            qDebug("Trying mutable database for \"%s\"", qPrintable(m_ticker));
            query = QSqlQuery(dbm);
            query.prepare(queryString);
            query.bindValue(":ticker", m_ticker);
            query.exec();
            query.next();
        }

        if(query.isValid())
        {
            QString company = query.value(0).toString();

            if(!company.isEmpty())
                emit done(company, query.value(1).toString(), query.value(2).toString());
            else
                emit done();
        }
        else
            emit done();
    }
}
