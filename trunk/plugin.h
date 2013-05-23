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

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QObject>
#include <QWidget>

class Plugin : public QObject
{
    Q_OBJECT

public:
    enum Type { AddTickersFrom, ExportTickersTo };

    Plugin();
    virtual ~Plugin();

    // plugin type
    virtual Type type() const = 0;

    virtual bool init()
    {
        return true;
    }

    void setTopLevelWidget(QWidget *w);
    QWidget *topLevelWidget() const;

private:
    QWidget *m_topLevelWidget;
};

inline
void Plugin::setTopLevelWidget(QWidget *w)
{
    m_topLevelWidget = w;
}

inline
QWidget *Plugin::topLevelWidget() const
{
    return m_topLevelWidget;
}

#define PLUGIN_CONSTRUCTOR(o)   \
extern "C"                      \
Plugin* plugin_create()         \
{                               \
    return (new (o));           \
}                               \
                                \
extern "C"                      \
void plugin_destroy(Plugin *pl) \
{                               \
    delete pl;                  \
}

#endif
