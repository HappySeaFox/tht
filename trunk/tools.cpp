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

#include <QDesktopWidget>
#include <QApplication>
#include <QWidget>
#include <QPoint>
#include <QRect>

#include <climits>

#include "tools.h"

const QPoint Tools::invalidQPoint(INT_MIN, INT_MIN);

void Tools::moveWindow(QWidget *w, const QPoint &pt)
{
    if(!w)
        return;

    const int sn = qApp->desktop()->screenCount();
    const QRect headGeometry = QRect(pt, QSize(w->width(), 20));

    for(int screen = 0;screen < sn;screen++)
    {
        QRect dr = qApp->desktop()->availableGeometry(screen);

        // move to a valid position
        if(dr.contains(headGeometry.topLeft()) || dr.contains(headGeometry.bottomRight()))
        {
            w->move(pt);
            break;
        }
    }
}
