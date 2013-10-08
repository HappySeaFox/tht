/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QNetworkRequest>
#include <QProgressBar>
#include <QByteArray>
#include <QUrl>

#include "stocksinplaydownloader.h"
#include "stocksinplaycookiejar.h"

StocksInPlayDownloader::StocksInPlayDownloader(const QUrl &url, QWidget *parent) :
    NetworkAccessDialog(parent)
{
    //: Stocks In Play - Stock Screener, http://stocksinplay.ru. It's ok not to translate "Stocks In Play" (e.g. you can just copy-paste "Stocks In Play" to your translation)
    setWindowTitle(tr("Stocks In Play"));
    //: Window title. Ticker is a short company name, see http://www.investopedia.com/terms/t/tickersymbol.asp . The common practice is to borrow "ticker" from English and incorporate into your language (see http://en.wikipedia.org/wiki/Loanword)
    setMessage(tr("Downloading tickers..."));
    setCookieJar(new StocksInPlayCookieJar(this));

    m_rxBase64Request = QRegExp("input\\s+type=\"hidden\"\\s+name=\"export\"\\s+value=\\s*\"([a-zA-Z0-9\\-+/=]+)\"");

    m_step = Page;

    // download tickers as simple CSV
    startRequest(QNetworkAccessManager::GetOperation, QNetworkRequest(url));
}

StocksInPlayDownloader::~StocksInPlayDownloader()
{}

bool StocksInPlayDownloader::finished()
{
    if(m_step == Page)
    {
        int pos = m_rxBase64Request.indexIn(data());
        QString base64 = m_rxBase64Request.cap(1);

        if(pos < 0 || base64.isEmpty())
        {
            showError(tr("Broken data"));
            return false;
        }

        QNetworkRequest request(QUrl("http://" STOCKSINPLAY "/export.php"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        m_step = Tickers;

        progressBar()->setRange(0, 0);
        progressBar()->setValue(-1);

        startRequest(QNetworkAccessManager::PostOperation,
                     request,
                     QByteArray("export=" + QUrl::toPercentEncoding(base64) + "&"));

        return false;
    }
    else
    {
        QStringList tickers = QString(data()).split(QRegExp("\\r?\\n"), QString::SkipEmptyParts);

        if(!tickers.isEmpty())
        {
            if(tickers.first() == "\"Ticker\"")
                tickers.takeFirst();

            foreach(QString t, tickers)
            {
                m_tickers.append(t.mid(1, t.length()-2));
            }
        }
    }

    return true;
}
