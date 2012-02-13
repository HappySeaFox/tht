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
#include <QTranslator>
#include <QtPlugin>
#include <QLocale>
#include <QIcon>
#include <QDir>

#include "tht.h"

Q_IMPORT_PLUGIN(qico)

int main(int argc, char *argv[])
{
    QtSingleApplication app(argc, argv);

    if(app.sendMessage("wake up"))
        return 0;

    // window icon
    app.setWindowIcon(QIcon(":/images/chart.ico"));

    QString locale = QLocale::system().name();

    qDebug("THT: Locale is \"%s\"", qPrintable(locale));

    // load translation
    QTranslator translator;
    qDebug("THT: Loading THT translation: %s", translator.load(locale, ":/ts") ? "ok" : "failed");

    QTranslator translator_qt;
    qDebug("THT: Loading Qt translation: %s", translator_qt.load("qt_" + locale, ":/ts") ? "ok" : "failed");

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
