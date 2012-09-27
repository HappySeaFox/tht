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

#include <QDesktopServices>
#include <QApplication>
#include <QDateTime>
#include <QtPlugin>
#include <QIcon>
#include <QDir>

Q_IMPORT_PLUGIN(qico)
Q_IMPORT_PLUGIN(qsqlite)

#include <cstdio>

#include "widget.h"

static void tickersDbOutput(QtMsgType type, const char *msg)
{
    Q_UNUSED(type)

    static QFile log(
                     QDesktopServices::storageLocation(QDesktopServices::TempLocation)
                     + QDir::separator()
                     + "tickersdb.log");

    static bool failed = false;

    fprintf(stderr, "TickersDb: %s\n", msg);

    if(!log.isOpen() && !failed)
    {
        failed = !log.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered);

        if(failed)
            fprintf(stderr, "TickersDb: Log file is unavailable\n");
    }

    if(!failed)
    {
        log.write(msg);
        log.write("\n");
    }
}

int main(int argc, char *argv[])
{
    setbuf(stderr, 0);

    qInstallMsgHandler(tickersDbOutput);

    qDebug("Starting at %s", qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

    QDir::setCurrent(TICKERS_DIR);

    qDebug("Tickers db dir: \"%s\"", TICKERS_DIR);

    QApplication a(argc, argv);

    Widget w;

    if(QApplication::arguments().indexOf("auto") >= 0)
        w.showMinimized();
    else
        w.show();

    return a.exec();
}
