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

#ifndef MASTERDATAEVENT_H
#define MASTERDATAEVENT_H

#include <QString>
#include <QEvent>

#include <windows.h>

static const int THT_MASTER_DATA_EVENT_TYPE = QEvent::User + 1;

class MasterDataEvent : public QEvent
{
public:
    MasterDataEvent(HWND hwnd = 0, const QString &ticker = QString());

    HWND hwnd() const
    {
        return m_hwnd;
    }

    QString ticker() const
    {
        return m_ticker;
    }

private:
    HWND m_hwnd;
    QString m_ticker;
};

#endif // MASTERDATAEVENT_H
