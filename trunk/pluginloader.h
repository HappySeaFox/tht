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
    Plugin*     (*plugin_create)();
    void        (*plugin_destroy)(Plugin *);
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

    QList<Plugin *> byType(Plugin::Type);

    void setTopLevelWidget(QWidget *w);
    QWidget *topLevelWidget() const;

private:
    PluginLoader(QObject *parent = 0);

private:
    QWidget *m_topLevelWidget;
};

inline
void PluginLoader::setTopLevelWidget(QWidget *w)
{
    m_topLevelWidget = w;

    iterator itEnd = end();

    for(iterator it = begin();it != itEnd;++it)
    {
        (*it).plugin->setTopLevelWidget(w);
    }
}

inline
QWidget *PluginLoader::topLevelWidget() const
{
    return m_topLevelWidget;
}

#endif