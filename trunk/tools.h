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

#ifndef TOOLS_H
#define TOOLS_H

#include <QByteArray>
#include <QString>
#include <QRegExp>
#include <QPoint>

#include <windows.h>

class QWidget;

/*
 *  Common tools
 */
class Tools
{
public:
    /*
     *  Move the window to the specified point
     */
    static void moveWindow(QWidget *w, const QPoint &pt);

    /*
     *  RealWindowFromPoint() from WIN32 API with fixes
     */
    static HWND RealWindowFromPoint(POINT pt);

    /*
     *  Is the specified window desktop (or background shell window)?
     */
    static bool isDesktop(HWND hwnd);

    /*
     *  Check for option 'opt' in the command line arguments
     */
    static bool hasOption(const QString &opt);

    /*
     *  Replace all whitespaces with &nbsp;
     */
    static QString nonBreakable(const QString &str);

    /*
     *  Show and raise the specified window
     */
    static void raiseWindow(QWidget *);

    /*
     *  Returns translated version of "Open YouTube tutorial"
     */
    static QString openYoutubeTutorialTitle();

    /*
     *  Returns translated version of "Downloading tickers..."
     */
    static QString downloadingTickersTitle();

    /*
     *  Returns translated version of "<Paste url here>"
     */
    static QString pasteUrlHereTitle();

    /*
     *  Returns translated version of "Tickers"
     */
    static QString tickersTitle();

    /*
     *  Returns translated version of "Add"
     */
    static QString addTitle();

    /*
     *  Returns translated version of "Error"
     */
    static QString errorTitle();

    /*
     *  Returns translated version of "Customize"
     */
    static QString customizeTitle();

    /*
     *  Returns translated version of "OK"
     */
    static QString oKTitle();

    /*
     *  Returns translated version of "Network error #%1"
     */
    static QString networkErrorTitle();

    /*
     *  Cell address validator
     */
    static QRegExp cellValidator();

    /*
     *  Regexp to validate a ticker name
     */
    static QRegExp tickerValidator();

    /*
     *  Encryption/decryption methods
     */
    static QByteArray encrypt(const QByteArray &data);
    static QByteArray decrypt(const QByteArray &data);

    /*
     *  Invalid QPoint
     */
    static const QPoint invalidQPoint;

private:
    Tools();

    static HWND FindBestChild(HWND hwndFound, POINT pt);
    static BOOL CALLBACK FindBestChildProc(HWND hwnd, LPARAM lParam);
};

#endif // TOOLS_H
