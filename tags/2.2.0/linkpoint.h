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

#ifndef LINKPOINT_H
#define LINKPOINT_H

#include <QMetaType>
#include <QString>
#include <QPoint>
#include <QList>

class QDataStream;

/*
 *  Compability class to support link points before 2.0.0
 */
struct LinkPoint
{
    LinkPoint(const QString &_name = QString(), const QList<QPoint> &_points = QList<QPoint>())
        : name(_name),
          points(_points)
    {}

    QString name;
    QList<QPoint> points;
};

Q_DECLARE_METATYPE(LinkPoint)
Q_DECLARE_METATYPE(QList<LinkPoint>)

// serialize/deserialize LinkPoint
QDataStream &operator<<(QDataStream &out, const LinkPoint &lp);
QDataStream &operator>>(QDataStream &in, LinkPoint &lp);

#endif // LINKPOINT_H
