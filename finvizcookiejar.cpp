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

#include <QUrl>

#include "finvizcookiejar.h"
#include "finvizurl.h"
#include "settings.h"

FinvizCookieJar::FinvizCookieJar(QObject *parent) :
    QNetworkCookieJar(parent)
{}

QList<QNetworkCookie> FinvizCookieJar::cookiesForUrl(const QUrl &url) const
{
    QList<QNetworkCookie> cookies;
    QString h = url.host().toLower();

    if(h == FINVIZ || h == FINVIZ_ELITE)
    {
        QString email = SETTINGS_GET_STRING(SETTING_FINVIZ_EMAIL);
        QString password = SETTINGS_GET_STRING(SETTING_FINVIZ_PASSWORD);

        if(!email.isEmpty() && !password.isEmpty())
        {
            QNetworkCookie cookieEmail("loginEmail", email.toLatin1());
            initCookie(&cookieEmail);
            cookies.append(cookieEmail);

            QNetworkCookie cookiePassword("loginPassword", password.toLatin1());
            initCookie(&cookiePassword);
            cookies.append(cookiePassword);
        }
    }

    return cookies;
}

void FinvizCookieJar::initCookie(QNetworkCookie *c) const
{
    if(!c)
        return;

    c->setDomain(QString(".") + FINVIZ);
    c->setPath("/");
}
