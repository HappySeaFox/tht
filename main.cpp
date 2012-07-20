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
#include <QDir>

#include <cstdlib>
#include <cstdio>

#include <windows.h>

#include "qtlockedfile.h"
#include "settings.h"
#include "tht.h"

static void thtOutput(QtMsgType type, const char *msg)
{
    Q_UNUSED(type)

    static QtLockedFile log(
                     QDesktopServices::storageLocation(QDesktopServices::TempLocation)
                     + QDir::separator()
                     + "tht.log");

    static bool failed = false;

    fprintf(stderr, "THT: %s\n", msg);

    if(!log.isOpen() && !failed)
    {
        failed = (!log.open(QIODevice::ReadWrite | QIODevice::Append)
                  || log.isLocked()
                  || !log.lock(QtLockedFile::WriteLock, false)
                  || !log.resize(0)
                  );

        if(failed)
            fprintf(stderr, "THT: Log file is unavailable\n");
    }

    if(!failed)
    {
        // truncate
        if(log.size() > 1*1024*1024) // 1 Mb
        {
            fprintf(stderr, "THT: Truncating log\n");

            log.seek(0);

            char buf[1024];
            int lines = 0;
            char c = 'x';

            while(log.readLine(buf, sizeof(buf)) > 0 && lines++ < 30)
            {}

            log.resize(log.pos());

            // check if '\n' is last
            if(log.seek(log.pos()-1))
            {
                log.getChar(&c);

                if(c != '\n')
                    log.write("\n");
            }

            log.write("...\n<overwrite>\n...\n");
        }

        log.write(msg);
        log.write("\n");
    }
}

static void copyDb()
{
    QString oldDb = Settings::instance()->mutableDatabasePath();
    QString newDb = oldDb + ".new";

    QString oldTs = oldDb + ".timestamp";
    QString newTs = oldTs + ".new";

    if(!QFile::exists(newDb) || !QFile::exists(newTs))
    {
        QFile::remove(newDb);
        QFile::remove(newTs);

        qDebug("No new database found locally");
        return;
    }

    qDebug("Copying new database");

    QFile::remove(oldDb);

    if(!QFile::copy(newDb, oldDb))
    {
        qDebug("Cannot copy new database");
        return;
    }

    QFile::remove(oldTs);

    if(!QFile::copy(newTs, oldTs))
        qDebug("Cannot copy new timestamp");

    QFile::remove(newDb);
    QFile::remove(newTs);

}

static void initializeDb()
{
    // reread database timestamps
    Settings::instance()->rereadTimestamps();

    // open ticker databases
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", Settings::instance()->mutableDatabaseName());
    db.setDatabaseName(Settings::instance()->mutableDatabasePath());

    if(!QFile::exists(db.databaseName()) || !db.isValid() || !db.open())
        qDebug("Cannot open mutable database (%s)", qPrintable(db.lastError().text()));
    else
        qDebug("Mutable database has been opened");

    db = QSqlDatabase::addDatabase("QSQLITE", Settings::instance()->persistentDatabaseName());
    db.setDatabaseName(Settings::instance()->persistentDatabasePath());

    if(!QFile::exists(db.databaseName()) || !db.isValid() || !db.open())
        qDebug("Cannot open persistent database (%s)", qPrintable(db.lastError().text()));
    else
        qDebug("Persistent database has been opened");
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

    // initialize ticker databases
    copyDb();
    initializeDb();

    // load translations
    QString locale = QLocale::system().name();

    QString ts = Settings::instance()->translation();
    QString dir = QCoreApplication::applicationDirPath() + QDir::separator() + "translations";

    qDebug("Locale \"%s\", translation \"%s\"", qPrintable(locale), qPrintable(ts));

    ts = ts.isEmpty() ? locale : (ts + ".qm");

    QTranslator translator;
    qDebug("Loading THT translation: %s", translator.load("tht_" + ts, dir) ? "ok" : "failed");

    QTranslator translator_qt;
    qDebug("Loading Qt translation: %s", translator_qt.load("qt_" + ts, dir) ? "ok" : "failed");

    app.installTranslator(&translator_qt);
    app.installTranslator(&translator);

    // main window
    THT w;
    w.show();

    app.setQuitOnLastWindowClosed(false);

    QObject::connect(&app, SIGNAL(messageReceived(const QString &)),
                        &w, SLOT(slotMessageReceived(const QString &)));

    int code = app.exec();

    qDebug("Goodbye at %s (exit code %d)", qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")), code);

    return code;
}
