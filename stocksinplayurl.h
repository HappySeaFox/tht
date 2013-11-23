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

#include <QMetaType>
#include <QString>
#include <QList>

class QDataStream;

struct StocksInPlayUrl
{
    StocksInPlayUrl(const QString &_name = QString(), const QString &_hash = QString())
        : name(_name),
          hash(_hash)
    {}

    QString name;
    QString hash;
};

Q_DECLARE_METATYPE(StocksInPlayUrl)
Q_DECLARE_METATYPE(QList<StocksInPlayUrl>)

// serialize/deserialize StocksInPlayUrl
QDataStream &operator<<(QDataStream &out, const StocksInPlayUrl &su);
QDataStream &operator>>(QDataStream &in, StocksInPlayUrl &su);

#define SETTINGS_GET_STOCKSINPLAY_HASHES Settings::instance()->binaryValue<QList<StocksInPlayUrl> >
#define SETTINGS_SET_STOCKSINPLAY_HASHES Settings::instance()->setBinaryValue<QList<StocksInPlayUrl> >

#define SETTING_STOCKSINPLAY_ID     "stocksinplay-id"
#define SETTING_STOCKSINPLAY_HASH   "stocksinplay-hash"
#define SETTING_STOCKSINPLAY_HASHES "stocksinplay-urls"

#endif // STOCKSINPLAYURL_H
