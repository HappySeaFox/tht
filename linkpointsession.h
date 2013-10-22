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

#ifndef LINKPOINTSESSION_H
#define LINKPOINTSESSION_H

#include <QDataStream>
#include <QMetaType>
#include <QString>
#include <QList>

class LinkedWindow;

struct LinkPointSession
{
    LinkPointSession(const QString &_name = QString(), const QList<LinkedWindow> &_windows = QList<LinkedWindow>())
        : name(_name),
          windows(_windows)
    {}

    QString name;
    QList<LinkedWindow> windows;
};

Q_DECLARE_METATYPE(LinkPointSession)
Q_DECLARE_METATYPE(QList<LinkPointSession>)

// serialize/deserialize LinkPointSession
QDataStream &operator<<(QDataStream &out, const LinkPointSession &lp);
QDataStream &operator>>(QDataStream &in, LinkPointSession &lp);

#endif // LINKPOINTSESSION_H
