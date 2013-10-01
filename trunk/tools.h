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

#ifndef TOOLS_H
#define TOOLS_H

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
     *  Cell address validator
     */
    static QRegExp cellValidator();

    /*
     *  Regexp to validate a ticker name
     */
    static QRegExp tickerValidator();

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
