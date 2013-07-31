/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REMOTEDATE_H
#define REMOTEDATE_H

#include <QDateTime>
#include <QString>

#include <windows.h>

class RemoteDatePrivate;

/*
 *  Class to get date and time in a foreign time zone
 */
class RemoteDate
{
public:
    RemoteDate(const QString &zone);
    ~RemoteDate();

    /*
     * Current date and time in timezone 'zone'
     */
    QDateTime dateTime();

private:
    TCHAR *stringToTChar(const QString &s) const;

private:
    RemoteDatePrivate *d;
};

#endif // REMOTEDATE_H
