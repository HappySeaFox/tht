/*
 * This file is part of THT-lib.
 *
 * THT-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation.
 *
 * THT-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCoreApplication>
#include <QDesktopWidget>
#include <QApplication>
#include <QRegExp>
#include <QWidget>
#include <QObject>
#include <QRect>

#include <climits>

#include "simplecrypt.h"
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

QString Tools::addTitle()
{
    //: This is the label on a button that user pushes to issue the command
    return QObject::tr("Add");
}

QString Tools::errorTitle()
{
    return QObject::tr("Error");
}

QString Tools::customizeTitle()
{
    //: This is the label on a button that user pushes to issue the command
    return QObject::tr("Customize");
}

QString Tools::oKTitle()
{
    //: This is the label on a button
    return QObject::tr("OK");
}

QString Tools::networkErrorTitle()
{
    //: %1 will be replaced with the error code by the application. It will look like "Network error #16"
    return QObject::tr("Network error #%1");
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

QByteArray Tools::encrypt(const QByteArray &data)
{
    if(data.isEmpty())
        return QByteArray();

    SimpleCrypt crypto(Q_UINT64_C(0xa71eded58d9db24c));
    crypto.setCompressionMode(SimpleCrypt::CompressionNever);
    crypto.setIntegrityProtectionMode(SimpleCrypt::ProtectionHash);

    QByteArray encoded = crypto.encryptToByteArray(data);

    return (crypto.lastError() == SimpleCrypt::ErrorNoError ? encoded : QByteArray());
}

QByteArray Tools::decrypt(const QByteArray &data)
{
    SimpleCrypt crypto(Q_UINT64_C(0xa71eded58d9db24c));

    QByteArray decoded = crypto.decryptToByteArray(data);

    return (crypto.lastError() == SimpleCrypt::ErrorNoError ? decoded : QByteArray());
}

Tools::Tools()
{}
