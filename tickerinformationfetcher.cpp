#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QWebSettings>
#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>
#include <QRegExp>

#include "tickerinformationfetcher.h"
#include "settings.h"

TickerInformationFetcher::TickerInformationFetcher(QObject *parent) :
    QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    m_reply = 0;
}

TickerInformationFetcher::~TickerInformationFetcher()
{
    deleteReply();
}

void TickerInformationFetcher::fetch(const QString &ticker)
{
    deleteReply();

    m_ticker = ticker;
    m_data.clear();

    qDebug("Starting a new network request for \"%s\"", qPrintable(ticker));

    QNetworkRequest request(QUrl(QString("http://finance.yahoo.com/q/in?s=%1").arg(ticker)));

    const OSVERSIONINFO version = Settings::instance()->version();

    request.setRawHeader("Dnt", "1");
    request.setRawHeader("User-Agent", QString("Mozilla/5.0 (%1 %2.%3; rv:10.0) Gecko/20100101 Firefox/10.0")
                         .arg(version.dwPlatformId == VER_PLATFORM_WIN32_NT ? "Windows NT" : "Windows")
                         .arg(version.dwMajorVersion)
                         .arg(version.dwMinorVersion).toAscii());

    m_reply = m_manager->get(request);

    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotNetworkError(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(slotNetworkDone()));
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(slotNetworkData()));
}

void TickerInformationFetcher::deleteReply()
{
    if(m_reply)
    {
        m_reply->blockSignals(true);
        m_reply->abort();
        delete m_reply;
    }
}

void TickerInformationFetcher::slotNetworkError(QNetworkReply::NetworkError err)
{
    qDebug("Network error #%d", err);
}

void TickerInformationFetcher::slotNetworkData()
{
    m_data += m_reply->readAll();
}

void TickerInformationFetcher::slotNetworkDone()
{
    qDebug("Network request done");

    QNetworkReply::NetworkError error = m_reply->error();

    m_reply->deleteLater();

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

    page.mainFrame()->setHtml(m_data);

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
