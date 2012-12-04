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
#include <QRect>

#include <climits>

#include "tools.h"

const QPoint Tools::invalidQPoint(INT_MIN, INT_MIN);

HWND Tools::hwndGlobal;
DWORD Tools::dwArea;

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
/*
 * FindBestChildProc
 * FindBestChild
 * RealWindowFromPoint
 *
 * are written by
 *
 * Copyright (c) 2002 by J Brown
 *
 * License: Freeware
 */
BOOL CALLBACK Tools::FindBestChildProc(HWND hwnd, LPARAM lParam)
{
    RECT rect;
    DWORD a;
    POINT pt;

    pt.x = (short)LOWORD(lParam);
    pt.y = (short)HIWORD(lParam);

    GetWindowRect(hwnd, &rect);

    if(PtInRect(&rect, pt))
    {
        a = (rect.right - rect.left) * (rect.bottom - rect.top);

        if(a < Tools::dwArea && IsWindowVisible(hwnd))
        {
            Tools::dwArea = a;
            Tools::hwndGlobal = hwnd;
        }
    }

    return TRUE;
}

HWND Tools::FindBestChild(HWND hwndFound, POINT pt)
{
    HWND  hwnd;
    DWORD dwStyle;

    Tools::dwArea = -1;	// Start off again
    Tools::hwndGlobal = 0;

    hwnd = GetParent(hwndFound);

    dwStyle = GetWindowLong(hwndFound, GWL_STYLE);

    if(hwnd == 0 || (dwStyle & WS_POPUP))
        hwnd = hwndFound;

    EnumChildWindows(hwnd, FindBestChildProc, MAKELPARAM(pt.x, pt.y));

    if(!Tools::hwndGlobal)
        Tools::hwndGlobal = hwnd;

    return Tools::hwndGlobal;
}

HWND Tools::RealWindowFromPoint(POINT pt)
{
    HWND hWndPoint = WindowFromPoint(pt);

    if(!hWndPoint)
        return 0;

    hWndPoint = FindBestChild(hWndPoint, pt);

    while(hWndPoint && !IsWindowVisible(hWndPoint))
        hWndPoint = GetParent(hWndPoint);

    return hWndPoint;
}