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

#include <QApplication>
#include <QIcon>
#include <QDir>

#include "widget.h"

int main(int argc, char *argv[])
{
    QDir::setCurrent(TICKERS_DIR);

    qDebug("Tickers db dir: \"%s\"", TICKERS_DIR);

    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/tickersdb.ico"));

    Widget w;

    if(QApplication::arguments().indexOf("auto") >= 0)
        w.showMinimized();
    else
        w.show();

    return a.exec();
}
