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
#include <QSqlDatabase>
#include <QDateTime>
#include <QSqlError>
#include <QLocale>
#include <QFile>
#include <QIcon>
#include <QDir>

#include <cstdlib>
#include <cstdio>

#include <windows.h>

#include "tickersdatabaseupdater.h"
#include "qtlockedfile.h"
#include "settings.h"
#include "tht.h"

static void thtOutput(QtMsgType type, const char *msg)
{
    static QtLockedFile log(
                     QDesktopServices::storageLocation(QDesktopServices::TempLocation)
                     + QDir::separator()
                     + "tht.log");

    static bool failed = false;

    fprintf(stderr, "THT: %s\n", msg);

    if(!log.isOpen() && !failed)
    {
        failed = (!log.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered)
                  || log.isLocked()
                  || !log.lock(QtLockedFile::WriteLock, false)
                  || !log.resize(0)
                  );

        if(failed)
            fprintf(stderr, "THT: Log file is unavailable\n");
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

    qInstallMsgHandler(thtOutput);

    qDebug("Starting at %s", qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

    QCoreApplication::setApplicationName("THT");
    QCoreApplication::setOrganizationName("THT");

    QtSingleApplication app(argc, argv);

    if(Settings::instance()->preloadMode())
    {
        qDebug("Preload mode");
        return 0;
    }

    AllowSetForegroundWindow(ASFW_ANY);

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

    new TickersDatabaseUpdater;

    // open current ticker databases
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", Settings::instance()->tickersMutableDatabaseName());
    db.setDatabaseName(Settings::instance()->tickersMutableDatabasePath());

    db = QSqlDatabase::addDatabase("QSQLITE", Settings::instance()->tickersPersistentDatabaseName());
    db.setDatabaseName(Settings::instance()->tickersPersistentDatabasePath());

    if(!QFile::exists(db.databaseName()) || !db.isValid() || !db.open())
        qDebug("Cannot open persistent database (%s)", qPrintable(db.lastError().text()));
    else
        qDebug("Database has been opened");

    THT w;
    w.show();

    app.setQuitOnLastWindowClosed(false);

    QObject::connect(&app, SIGNAL(messageReceived(const QString &)),
                        &w, SLOT(slotMessageReceived(const QString &)));

    int code = app.exec();

    qDebug("Goodbye at %s (exit code %d)", qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")), code);

    return code;
}
