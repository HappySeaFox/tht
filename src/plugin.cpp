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

#include <QApplication>
#include <QWidget>

#include "plugin.h"

class PluginPrivate
{
public:
    PluginPrivate()
    {}
};

/****************************************/

Plugin::Plugin() : QObject()
{
    d = new PluginPrivate;
}

Plugin::~Plugin()
{
    delete d;
}

bool Plugin::init()
{
    return true;
}

QWidget *Plugin::topLevelWidget() const
{
    // this is not thread-safe, but do we need a thread safety for this at all?
    static QWidget *topLevelWidget = 0;

    if(!topLevelWidget)
    {
        QWidgetList wl = QApplication::topLevelWidgets();

        foreach(QWidget *w, wl)
        {
            if(!qstrcmp(w->metaObject()->className(), "THT"))
            {
                topLevelWidget = w;
                break;
            }
        }
    }

    return topLevelWidget;
}

QWidget *Plugin::containerLeft() const
{
    static QWidget *c = 0;

    if(!c)
    {
        QWidget *tht = topLevelWidget();

        if(tht)
            c = tht->findChild<QWidget *>("containerLeft");
    }

    return c;
}

QWidget *Plugin::containerRight() const
{
    static QWidget *c = 0;

    if(!c)
    {
        QWidget *tht = topLevelWidget();

        if(tht)
            c = tht->findChild<QWidget *>("containerRight");
    }

    return c;
}
