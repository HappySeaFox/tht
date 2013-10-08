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

#include <QCoreApplication>
#include <QDesktopWidget>
#include <QApplication>
#include <QRegExp>
#include <QWidget>
#include <QObject>
#include <QRect>

#include <climits>

#include "tools.h"

const QPoint Tools::invalidQPoint(INT_MIN, INT_MIN);

class ToolsPrivate
{
public:
    static HWND hwndGlobal;
    static DWORD dwArea;
};

HWND ToolsPrivate::hwndGlobal;
DWORD ToolsPrivate::dwArea;

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

        if(a < ToolsPrivate::dwArea && IsWindowVisible(hwnd))
        {
            ToolsPrivate::dwArea = a;
            ToolsPrivate::hwndGlobal = hwnd;
        }
    }

    return TRUE;
}

HWND Tools::FindBestChild(HWND hwndFound, POINT pt)
{
    HWND hwnd;
    DWORD dwStyle;

    ToolsPrivate::dwArea = -1;
    ToolsPrivate::hwndGlobal = 0;

    hwnd = GetParent(hwndFound);

    dwStyle = GetWindowLong(hwndFound, GWL_STYLE);

    if(!hwnd || (dwStyle & WS_POPUP))
        hwnd = hwndFound;

    EnumChildWindows(hwnd, FindBestChildProc, MAKELPARAM(pt.x, pt.y));

    if(!ToolsPrivate::hwndGlobal)
        ToolsPrivate::hwndGlobal = hwnd;

    return ToolsPrivate::hwndGlobal;
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

bool Tools::isDesktop(HWND hwnd)
{
    return IsWindow(hwnd) && (hwnd == GetDesktopWindow() || hwnd == GetShellWindow());
}

bool Tools::hasOption(const QString &opt)
{
    return (QCoreApplication::arguments().indexOf(opt) >= 0);
}

QString Tools::nonBreakable(const QString &str)
{
    return QString(str).replace(QRegExp("\\s"), "&nbsp;");
}

void Tools::raiseWindow(QWidget *w)
{
    if(!w)
        return;

    w->show();
    w->setWindowState(w->windowState() & ~Qt::WindowMinimized);
    w->raise();
}

QString Tools::openYoutubeTutorialTitle()
{
    //: This is the label on a menu item that user clicks to issue the command. Means "Open a video tutorial on YouTube"
    return QObject::tr("Open YouTube tutorial");
}

QString Tools::downloadingTickersTitle()
{
    //: Window title. Ticker is a short company name, see http://www.investopedia.com/terms/t/tickersymbol.asp . The common practice is to borrow "ticker" from English and incorporate into your language (see http://en.wikipedia.org/wiki/Loanword)
    return QObject::tr("Downloading tickers...");
}

QString Tools::pasteUrlHereTitle()
{
    //: Command addressed to the user. Means "Please paste some URL here". "URL" can be translated as "link"
    return QObject::tr("<Paste url here>");
}

QString Tools::tickersTitle()
{
    //: Ticker is a short company name, see http://www.investopedia.com/terms/t/tickersymbol.asp . The common practice is to borrow "ticker" from English and incorporate into your language (see http://en.wikipedia.org/wiki/Loanword)
    return QObject::tr("Tickers");
}

QRegExp Tools::cellValidator()
{
    static QRegExp rx("([a-zA-Z]+)([0-9]+)");
    return rx;
}

QRegExp Tools::tickerValidator()
{
    static QRegExp rx("[a-zA-Z0-9\\-\\.\\$]{1,8}");
    return rx;
}

Tools::Tools()
{}
