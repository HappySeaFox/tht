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

#include <QApplication>
#include <QWidget>
#include <QTimer>

#include "plugin.h"

class PluginPrivate
{
public:
    PluginPrivate()
    {
    }
};

/****************************************/

Plugin::Plugin() : QObject()
{
    d = new PluginPrivate;

    QTimer::singleShot(0, this, SLOT(delayedInit()));
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

void Plugin::delayedInit()
{
}
