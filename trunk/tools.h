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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TOOLS_H
#define TOOLS_H

#include <QPoint>

#include <windows.h>

class QWidget;

class Tools
{
public:
    static void moveWindow(QWidget *w, const QPoint &pt);
    static HWND RealWindowFromPoint(POINT pt);
    static bool isDesktop(HWND hwnd);

    static bool hasOption(const QString &);

    static const QPoint invalidQPoint;

private:
    Tools();

    static HWND FindBestChild(HWND hwndFound, POINT pt);
    static BOOL CALLBACK FindBestChildProc(HWND hwnd, LPARAM lParam);
};

#endif // TOOLS_H
