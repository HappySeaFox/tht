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

#include <QMutableListIterator>
#include <QByteArray>
#include <QUrl>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QUrlQuery>
#endif

#include "finvizdownloader.h"
#include "finvizcookiejar.h"
#include "csvreader.h"

static const int REQUIRED_FIELDS = 11;

FinvizDownloader::FinvizDownloader(const QUrl &url, QWidget *parent) :
    DataDownloader(parent)
{
    //: Finviz - Stock Screener, http://finviz.com. It's ok not to translate "Finviz" (e.g. you can just copy-paste "Finviz" to your translation)
    setWindowTitle(tr("Finviz"));
    //: Ticker is a short company name, see http://www.investopedia.com/terms/t/tickersymbol.asp . The common practice is to transliterate "ticker" to your language
    setMessage(tr("Downloading tickers..."));
    setCookieJar(new FinvizCookieJar(this));

    QUrl u = url;

    // fix path
    u.setPath(u.path().replace("screener.ashx", "export.ashx"));

    QList<QPair<QString, QString> > items;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QUrlQuery uq(u);
    items = uq.queryItems();
#else
    items = u.queryItems();
#endif

    QMutableListIterator<QPair<QString, QString> > i(items);

    // fix "v" query item
    while(i.hasNext())
    {
        QPair<QString, QString> pair = i.next();

        if(pair.first == "v")
        {
            if(pair.second != "151")
            {
                pair.second = "151";
                i.setValue(pair);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                uq.setQueryItems(items);
#else
                u.setQueryItems(items);
#endif
            }

            break;
        }
    }

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    u.setQuery(uq);
#endif

    // download tickers as CSV
    get(u);
}

FinvizDownloader::~FinvizDownloader()
{}

bool FinvizDownloader::finished()
{
    // parse CSV
    CsvReader csv(data());

    QStringList str;
    csv.parseLine();

    while(!(str = csv.parseLine()).isEmpty())
    {
        if(str.size() != REQUIRED_FIELDS)
        {
            showError(tr("Broken data (fields: %1, required: %2)").arg(str.size()).arg(REQUIRED_FIELDS));
            return false;
        }

        m_tickers.append(str[1]);
    }

    return true;
}
