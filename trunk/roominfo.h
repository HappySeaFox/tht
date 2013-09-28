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

#ifndef ROOMINFO_H
#define ROOMINFO_H

#include <QMetaType>
#include <QString>
#include <QList>

class QDataStream;

struct RoomInfo
{
    RoomInfo(const QString &_jid = QString(), const QString &_nick = QString(), const QString &_password = QString()) :
        jid(_jid),
        password(_password),
        nick(_nick)
    {}

    QString jid;
    QString nick;
    QString password;
};

Q_DECLARE_METATYPE(RoomInfo)
Q_DECLARE_METATYPE(QList<RoomInfo>)

#define SETTINGS_GET_ROOMS_INFO Settings::instance()->value<QList<RoomInfo> >
#define SETTINGS_SET_ROOMS_INFO Settings::instance()->setValue<QList<RoomInfo> >

QDataStream &operator<<(QDataStream &out, const RoomInfo &ri);
QDataStream &operator>>(QDataStream &in, RoomInfo &ri);

#endif // ROOMINFO_H
