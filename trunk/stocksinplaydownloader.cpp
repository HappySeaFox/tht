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
#include "stocksinplaytools.h"
#include "tools.h"

StocksInPlayDownloader::StocksInPlayDownloader(const QString &hash, QWidget *parent) :
    NetworkAccessDialog(parent)
{
    setWindowTitle(StocksInPlayTools::stocksInPlayTitle());
    setMessage(Tools::downloadingTickersTitle());
    setCookieJar(new StocksInPlayCookieJar(this));

    // download tickers as simple CSV
    QNetworkRequest request(QUrl("http://" STOCKSINPLAY "/export.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    startRequest(QNetworkAccessManager::PostOperation,
                 request,
                 QByteArray("export=" + QUrl::toPercentEncoding(hash) + "&"));
}

StocksInPlayDownloader::~StocksInPlayDownloader()
{}

bool StocksInPlayDownloader::finished()
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

    return true;
}
