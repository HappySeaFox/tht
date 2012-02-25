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

#include <QtSingleApplication>
#include <QDesktopServices>
#include <QTranslator>
#include <QtPlugin>
#include <QLocale>
#include <QIcon>
#include <QDir>

#include <cstdlib>

#include "tht.h"

void myMessageOutput(QtMsgType type, const char *msg)
{
    static QFile log(QDesktopServices::storageLocation(QDesktopServices::TempLocation)
                     + QDir::separator()
                     + "tht.log");

    if(!log.isOpen())
        log.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered);

    switch (type)
    {
        case QtDebugMsg:
            fprintf(stderr, "%s\n", msg);
            log.write(msg);
            log.write("\n");
        break;

        case QtWarningMsg:
            fprintf(stderr, "%s\n", msg);
            log.write(msg);
            log.write("\n");
        break;

        case QtCriticalMsg:
            fprintf(stderr, "%s\n", msg);
            log.write(msg);
            log.write("\n");
        break;

        case QtFatalMsg:
            fprintf(stderr, "%s\n", msg);
            log.write(msg);
            log.write("\n");
            abort();
        break;
    }
}

int main(int argc, char *argv[])
{
    qInstallMsgHandler(myMessageOutput);

    QtSingleApplication app(argc, argv);

    if(app.sendMessage("wake up"))
        return 0;

    // window icon
    app.setWindowIcon(QIcon(":/images/chart.ico"));

    QString locale = QLocale::system().name();

    qDebug("THT: Locale is \"%s\"", qPrintable(locale));

    QString dir = QCoreApplication::applicationDirPath() + QDir::separator() + "translations";

    qDebug("THT: Application directory: %s", qPrintable(dir));

    // load translation
    QTranslator translator;
    qDebug("THT: Loading THT translation: %s", translator.load(locale, dir) ? "ok" : "failed");

    QTranslator translator_qt;
    qDebug("THT: Loading Qt translation: %s", translator_qt.load("qt_" + locale, dir) ? "ok" : "failed");

    app.installTranslator(&translator_qt);
    app.installTranslator(&translator);

    QApplication::setApplicationName("THT");
    QApplication::setOrganizationName("Noname");

    THT w;
    w.show();

    app.setActivationWindow(&w);
    app.setQuitOnLastWindowClosed(false);

    QObject::connect(&app, SIGNAL(messageReceived(const QString &)),
                        &w, SLOT(slotMessageReceived(const QString &)));

    return app.exec();
}
