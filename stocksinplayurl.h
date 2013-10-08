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

#ifndef STOCKSINPLAYURL_H
#define STOCKSINPLAYURL_H

#include <QDataStream>
#include <QMetaType>
#include <QString>
#include <QList>
#include <QUrl>

struct StocksInPlayUrl
{
    StocksInPlayUrl(const QString &_name = QString(), const QUrl &_url = QUrl())
        : name(_name),
          url(_url)
    {}

    QString name;
    QUrl url;
};

Q_DECLARE_METATYPE(StocksInPlayUrl)
Q_DECLARE_METATYPE(QList<StocksInPlayUrl>)

#define SETTINGS_GET_STOCKSINPLAY_URLS Settings::instance()->value<QList<StocksInPlayUrl> >
#define SETTINGS_SET_STOCKSINPLAY_URLS Settings::instance()->setValue<QList<StocksInPlayUrl> >

#define SETTING_STOCKSINPLAY_ID   "stocksinplay-id"
#define SETTING_STOCKSINPLAY_HASH "stocksinplay-hash"
#define SETTING_STOCKSINPLAY_URLS "stocksinplay-urls"

#endif // STOCKSINPLAYURL_H
