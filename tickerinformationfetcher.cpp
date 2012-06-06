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
    m_net->get(QUrl(QString("http://finance.yahoo.com/q/in?s=%1").arg(ticker)));
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
    QString tmp;
    int index;

    page.settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    page.settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    page.settings()->setAttribute(QWebSettings::JavaEnabled, false);
    page.settings()->setAttribute(QWebSettings::PluginsEnabled, false);

    page.mainFrame()->setHtml(m_net->data());

    // ticker name
    QWebElementCollection elements = page.mainFrame()->findAllElements("div.title");

    QRegExp rx("\\s*\\(" + QRegExp::escape(m_ticker) + "\\)$");

    foreach(QWebElement div, elements)
    {
        tmp = div.findFirst("h2").toPlainText();
        index = rx.indexIn(tmp);

        if(index >= 0)
        {
            name = tmp.left(index);
            break;
        }
    }

    // sector & industry
    elements = page.mainFrame()->findAllElements("th.yfnc_tablehead1");

    foreach(QWebElement th, elements)
    {
        if(sector.isEmpty() && th.toPlainText() == "Sector:")
            sector = th.nextSibling().findFirst("a").toPlainText();

        if(industry.isEmpty() && th.toPlainText() == "Industry:")
            industry = th.nextSibling().findFirst("a").toPlainText();

        // everything is found
        if(!sector.isEmpty() && !industry.isEmpty())
            break;
    }

    emit done(QString(), name, sector, industry);
}
