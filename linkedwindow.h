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

#ifndef LINKEDWINDOW_H
#define LINKEDWINDOW_H

#include <QByteArray>
#include <QMetaType>
#include <QPoint>

class QDataStream;

struct LinkedWindow
{
    LinkedWindow(bool _master = false, const QPoint &_point = QPoint(), const QByteArray &_extraData = QByteArray())
        : master(_master),
          point(_point),
          extraData(_extraData)
    {}

    bool master;
    QPoint point;
    QByteArray extraData; // some extra data specific to the link
};

Q_DECLARE_METATYPE(LinkedWindow)
Q_DECLARE_METATYPE(QList<LinkedWindow>)

// serialize/deserialize LinkedWindow
QDataStream &operator<<(QDataStream &out, const LinkedWindow &lw);
QDataStream &operator>>(QDataStream &in, LinkedWindow &lw);

#endif // LINKEDWINDOW_H
