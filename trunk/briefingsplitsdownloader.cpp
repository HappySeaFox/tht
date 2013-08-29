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

#include <QRegExp>
#include <QUrl>

#include "briefingsplitsdownloader.h"
#include "settings.h"

BriefingSplitsDownloader::BriefingSplitsDownloader(const QUrl &url, QWidget *parent) :
    DataDownloader(parent)
{
    //: It's ok not to translate "Briefing Stock Splits" (e.g. just copy-paste "Briefing Stock Splits" to your translation)
    setWindowTitle(tr("Briefing Stock Splits"));
    setMessage(tr("Downloading tickers..."));
    get(url);
}

BriefingSplitsDownloader::~BriefingSplitsDownloader()
{}

bool BriefingSplitsDownloader::finished()
{
    QRegExp rx(QString("<a\\s+class=\"story-link-title\"\\s+[^>]+>(%1)</a>").arg(Settings::instance()->tickerValidator().pattern()));
    QString d = data();

    int pos = 0;

    while((pos = rx.indexIn(d, pos)) >= 0)
    {
        m_tickers.append(rx.cap(1));
        pos += rx.matchedLength();
    }

    return true;
}
