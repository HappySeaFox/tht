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

#include <QNetworkCookie>
#include <QUrl>

#include "stocksinplaycookiejar.h"
#include "stocksinplaytools.h"
#include "stocksinplayurl.h"
#include "settings.h"

StocksInPlayCookieJar::StocksInPlayCookieJar(QObject *parent) :
    QNetworkCookieJar(parent)
{}

QList<QNetworkCookie> StocksInPlayCookieJar::cookiesForUrl(const QUrl &url) const
{
    QList<QNetworkCookie> cookies;
    QString h = url.host().toLower();

    if(h == STOCKSINPLAY)
    {
        QString id = SETTINGS_GET_STRING(SETTING_STOCKSINPLAY_ID);
        QString hash = StocksInPlayTools::cachedHash();

        if(!id.isEmpty() && !hash.isEmpty())
        {
            QNetworkCookie cookieId("id", id.toLatin1());
            initCookie(&cookieId);
            cookies.append(cookieId);

            QNetworkCookie cookieHash("hash", hash.toLatin1());
            initCookie(&cookieHash);
            cookies.append(cookieHash);
        }
    }

    return cookies;
}

void StocksInPlayCookieJar::initCookie(QNetworkCookie *c) const
{
    if(!c)
        return;

    c->setDomain(STOCKSINPLAY);
    c->setPath("/");
}
