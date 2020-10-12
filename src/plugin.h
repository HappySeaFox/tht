/*
 * This file is part of THT-lib.
 *
 * THT-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation.
 *
 * THT-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QVariant>
#include <QString>
#include <QObject>
#include <QWidget>

class PluginPrivate;

// set by the plugin, see THT_PLUGIN_INTERFACE_IMPLEMENTATION below
#define THT_PLUGIN_PROPERTY_NAME             "name"
#define THT_PLUGIN_PROPERTY_AUTHOR           "author"
#define THT_PLUGIN_PROPERTY_VERSION          "version"
#define THT_PLUGIN_PROPERTY_URL              "url"
#define THT_PLUGIN_PROPERTY_UUID             "uuid"
#define THT_PLUGIN_PROPERTY_DEPRECATES_UUIDS "deprecates-uuids"
#define THT_PLUGIN_PROPERTY_LICENSE_TEXT     "license-text"

// set by the plugin loader
#define THT_PLUGIN_PROPERTY_LICENSE_FILE "license-file"
#define THT_PLUGIN_PROPERTY_FILENAME     "filename"

/*
 *  Base class for all plugins
 */
class Plugin : public QObject
{
    Q_OBJECT

public:
    /*
     *  supported plugin types
     */
    enum Type { Common, AddTickersFrom, ExportTickersTo };

    Plugin();
    virtual ~Plugin();

    /*
     *  initialize plugin. This method is optional, you can
     *  not implement it
     */
    virtual bool init();

    /*
     *  plugin type. This method is required
     */
    virtual Type type() const = 0;

protected:
    /*
     *  Pointer to the THT window. You can use this pointer
     *  as a parent widget in your dialogs. Please remember
     *  that the plugin loader initializes this pointer just
     *  before calling init(), so never try to use it in the
     *  plugin's constructor
     */
    QWidget *topLevelWidget() const;

    /*
     *  Pointers to left-aligned and right-aligned containers for
     *  plugins' widgets. The containters are created in the bootom of the
     *  THT window like that:
     *
     *  |                                   |
     *  |    +----------++----------+       |
     *  | +  | LEFT     ||    RIGHT |   \/  |
     *  |    +----------++----------+       |
     *  +-----------------------------------+
     *
     *
     *  Any plugin can embed widgets to them
     */
    QWidget *containerLeft() const;
    QWidget *containerRight() const;

signals:
    void openTicker(const QString &);

private:
    PluginPrivate *d;
};

#ifdef Q_OS_WIN
#define PLUGUN_EXPORT __declspec(dllexport)
#else
#define PLUGUN_EXPORT
#endif

#define THT_PLUGIN_CONSTRUCTOR(o) \
extern "C" PLUGUN_EXPORT          \
Plugin* plugin_create()           \
{                                 \
    return (new (o));             \
}                                 \
                                  \
extern "C" PLUGUN_EXPORT          \
void plugin_destroy(Plugin *pl)   \
{                                 \
    delete pl;                    \
}

#define THT_PLUGIN_INTERFACE_IMPLEMENTATION                                     \
setProperty(THT_PLUGIN_PROPERTY_NAME, THT_PLUGIN_NAME);                         \
setProperty(THT_PLUGIN_PROPERTY_AUTHOR, THT_PLUGIN_AUTHOR);                     \
setProperty(THT_PLUGIN_PROPERTY_VERSION, THT_PLUGIN_VERSION);                   \
setProperty(THT_PLUGIN_PROPERTY_URL, THT_PLUGIN_URL);                           \
setProperty(THT_PLUGIN_PROPERTY_UUID, THT_PLUGIN_UUID);                         \
setProperty(THT_PLUGIN_PROPERTY_DEPRECATES_UUIDS, THT_PLUGIN_DEPRECATES_UUIDS); \
setProperty(THT_PLUGIN_PROPERTY_LICENSE_TEXT, THT_PLUGIN_LICENSE_TEXT);

#endif // PLUGIN_H
