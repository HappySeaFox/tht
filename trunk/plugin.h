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

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QObject>
#include <QWidget>

class PluginPrivate;

// set by the plugin, see THT_PLUGIN_INTERFACE_IMPLEMENTATION below
#define THT_PLUGIN_PROPERTY_NAME         "name"
#define THT_PLUGIN_PROPERTY_AUTHOR       "author"
#define THT_PLUGIN_PROPERTY_VERSION      "version"
#define THT_PLUGIN_PROPERTY_URL          "url"
#define THT_PLUGIN_PROPERTY_UUID         "uuid"

// set by the plugin loader
#define THT_PLUGIN_PROPERTY_LICENSE_FILE "license"
#define THT_PLUGIN_PROPERTY_FILENAME     "filename"

class Plugin : public QObject
{
    Q_OBJECT

public:
    enum Type { Common, AddTickersFrom, ExportTickersTo };

    Plugin();
    virtual ~Plugin();

    virtual bool init();

    // plugin type
    virtual Type type() const = 0;

protected:
    QWidget *topLevelWidget() const;

protected slots:
    virtual void delayedInit();

private:
    PluginPrivate *d;
};

#ifdef Q_OS_WIN
#define PLUGUN_EXPORT __declspec(dllexport)
#else
#define PLUGUN_EXPORT
#endif

#define PLUGIN_CONSTRUCTOR(o)   \
extern "C" PLUGUN_EXPORT        \
Plugin* plugin_create()         \
{                               \
    return (new (o));           \
}                               \
                                \
extern "C" PLUGUN_EXPORT        \
void plugin_destroy(Plugin *pl) \
{                               \
    delete pl;                  \
}

#define THT_PLUGIN_INTERFACE_IMPLEMENTATION                   \
setProperty(THT_PLUGIN_PROPERTY_NAME, THT_PLUGIN_NAME);       \
setProperty(THT_PLUGIN_PROPERTY_AUTHOR, THT_PLUGIN_AUTHOR);   \
setProperty(THT_PLUGIN_PROPERTY_VERSION, THT_PLUGIN_VERSION); \
setProperty(THT_PLUGIN_PROPERTY_URL, THT_PLUGIN_URL);         \
setProperty(THT_PLUGIN_PROPERTY_UUID, THT_PLUGIN_UUID);

#endif
