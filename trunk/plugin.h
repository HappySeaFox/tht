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

#include <QString>
#include <QObject>
#include <QWidget>
#include <QList>
#include <QMap>

class QMenu;

struct Hotkey
{
    Hotkey(const Qt::Key &_key = Qt::Key_0,
           const Qt::KeyboardModifiers &_modifiers = Qt::NoModifier)
        : key(_key), modifiers(_modifiers)
    {}

    Qt::Key key;
    Qt::KeyboardModifiers modifiers;
};

inline
bool operator== (const Hotkey &a, const Hotkey &b)
{
    return a.key == b.key
            && a.modifiers == b.modifiers;
}

class Plugin : public QObject
{
    Q_OBJECT

public:
    enum Type { AddTickersFrom, ExportTickersTo };

    Plugin();
    virtual ~Plugin();

    // plugin type
    virtual Type type() const = 0;

    // menu for list N
    virtual bool embed(int list, QMenu *menu) = 0;

    virtual bool init()
    {
        return true;
    }

    virtual QList<Hotkey> supportedHotkeysInList() const
    {
        return QList<Hotkey>();
    }

    virtual void listHotkeyActivated(int list, const Hotkey &ke)
    {
        Q_UNUSED(list)
        Q_UNUSED(ke)
    }

    void setTopLevelWidget(QWidget *w);
    QWidget *topLevelWidget() const;

protected:
    int senderStandaloneActionToList() const;
    int senderMenuActionToList() const;

signals:
    void tickers(int list, const QStringList &tickers);

protected:
    typedef QMap<int, QObject *> Embeds;
    Embeds m_embeds;

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
