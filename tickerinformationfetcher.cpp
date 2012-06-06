#include <QWebElementCollection>
#include <QWebSettings>
#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>
#include <QRegExp>
#include <QUrl>

#include "tickerinformationfetcher.h"
#include "networkaccess.h"

TickerInformationFetcher::TickerInformationFetcher(QObject *parent) :
    QObject(parent)
{
    m_net = new NetworkAccess(this);

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));
}

void TickerInformationFetcher::fetch(const QString &ticker)
{
    m_ticker = ticker;
    m_net->get(QUrl(QString("http://finviz.com/quote.ashx?t=%1&ty=l&ta=0&p=d").arg(ticker)));
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
