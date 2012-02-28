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
#include <QDateTime>
#include <QtPlugin>
#include <QLocale>
#include <QIcon>
#include <QDir>

#include <cstdlib>
#include <cstdio>

#include "settings.h"
#include "tht.h"

void myMessageOutput(QtMsgType type, const char *msg)
{
    static QFile log(QDesktopServices::storageLocation(QDesktopServices::TempLocation)
                     + QDir::separator()
                     + "tht.log");

    static bool failed = false;

    if(!log.isOpen() && !failed)
        failed = !log.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered);

    fprintf(stderr, "THT: %s\n", msg);

    log.write(msg);
    log.write("\n");

    if(type == QtFatalMsg)
        abort();
}

int main(int argc, char *argv[])
{
    setbuf(stderr, 0);

    qInstallMsgHandler(myMessageOutput);

    qDebug("Starting at %s", qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

    QCoreApplication::setApplicationName("THT");
    QCoreApplication::setOrganizationName("Noname");

    QtSingleApplication app(argc, argv);

    if(Settings::instance()->preloadMode())
    {
        qDebug("Preload mode");
        return 0;
    }

    if(app.sendMessage("wake up"))
        return 0;

    // window icon
    app.setWindowIcon(QIcon(":/images/chart.ico"));

    QString locale = QLocale::system().name();

    qDebug("Locale is \"%s\"", qPrintable(locale));

    QString dir = QCoreApplication::applicationDirPath() + QDir::separator() + "translations";

    // load translation
    QTranslator translator;
    qDebug("Loading THT translation: %s", translator.load(locale, dir) ? "ok" : "failed");

    QTranslator translator_qt;
    qDebug("Loading Qt translation: %s", translator_qt.load("qt_" + locale, dir) ? "ok" : "failed");

    app.installTranslator(&translator_qt);
    app.installTranslator(&translator);

    THT w;
    w.show();

    app.setActivationWindow(&w);
    app.setQuitOnLastWindowClosed(false);

    QObject::connect(&app, SIGNAL(messageReceived(const QString &)),
                        &w, SLOT(slotMessageReceived(const QString &)));

    return app.exec();
}
