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

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QString>
#include <QWidget>
#include <QList>

#include "plugin.h"

class QTranslator;
class QLibrary;

struct PluginLibrary
{
    PluginLibrary() : library(0),
                      plugin_create(0),
                      plugin_destroy(0),
                      plugin(0),
                      translator(0)
    {}

    QLibrary    *library;

    typedef Plugin* (*PluginCreateFunc)();
    typedef void (*PluginDestroyFunc)(Plugin *);

    PluginCreateFunc plugin_create;
    PluginDestroyFunc plugin_destroy;

    Plugin      *plugin;
    QTranslator *translator;
};

typedef QList<PluginLibrary> PluginLibraryList;

class PluginLoader : public QObject, private PluginLibraryList
{
    Q_OBJECT

public:
    static PluginLoader *instance();

    ~PluginLoader();

    void init();

    void unload();

    QList<Plugin *> byType(Plugin::Type) const;

    Plugin* byUuid(const QString &uuid) const;

signals:
    void openTicker(const QString &);

private:
    PluginLoader(QObject *parent = 0);
};

#endif // PLUGINLOADER_H
