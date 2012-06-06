#include <QWebElementCollection>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QWebSettings>
#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>
#include <QRegExp>
#include <QTimer>
#include <QUrl>

#include "tickerinformationfetcher.h"
#include "networkaccess.h"
#include "settings.h"

TickerInformationFetcher::TickerInformationFetcher(QObject *parent) :
    QObject(parent)
{
    m_net = new NetworkAccess(this);

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));

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
        QString queryString = "SELECT company, sector, industry FROM tickers WHERE ticker = '" + m_ticker + "'";

        QSqlQuery query(queryString, dbp);

        if(!query.next())
        {
            query = QSqlQuery(queryString, dbm);
            query.next();
        }

        if(query.isValid())
        {
            QString company = query.value(0).toString();

            if(!company.isEmpty())
            {
                emit done(QString(), company, query.value(1).toString(), query.value(2).toString());
                return;
            }
        }
    }

    m_net->get(QUrl(QString("http://finviz.com/quote.ashx?t=%1&ty=l&ta=0&p=d").arg(m_ticker)));
}

void TickerInformationFetcher::slotFinished()
{
    QNetworkReply::NetworkError error = m_net->error();

    if(error != QNetworkReply::NoError)
    {
        emit done(error == QNetworkReply::UnknownContentError
                  ? tr("Not found")
                  : tr("Network error #%1").arg(error));
        return;
    }

    QWebPage page;
    QString name, sector, industry;

    page.settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    page.settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    page.settings()->setAttribute(QWebSettings::JavaEnabled, false);
    page.settings()->setAttribute(QWebSettings::PluginsEnabled, false);

    page.mainFrame()->setHtml(m_net->data());

    // ticker name
    QWebElementCollection elements = page.mainFrame()->findAllElements("table.fullview-title");

    QRegExp rx("\\s*\\(" + QRegExp::escape(m_ticker) + "\\)$");

    foreach(QWebElement table, elements)
    {
        QWebElement tr = table.findFirst("tr");

        name = tr.findFirst("td").findFirst("a.fullview-ticker").toPlainText();

        if(name!= m_ticker)
            continue;

        tr = tr.nextSibling();
        name = tr.findFirst("td").findFirst("a.tab-link").toPlainText();

        tr = tr.nextSibling();
        QWebElement a = tr.findFirst("td").findFirst("a.tab-link");
        sector = a.toPlainText();
        industry = a.nextSibling().toPlainText();

        break;
    }

    emit done(QString(), name, sector, industry);
}
