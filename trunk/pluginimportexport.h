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

#ifndef PLUGINIMPORTEXPORT_H
#define PLUGINIMPORTEXPORT_H

#include <QString>
#include <QList>
#include <QMap>

#include "plugin.h"
#include "hotkey.h"

class QMenu;

class PluginImportExportPrivate;

class PluginImportExport : public Plugin
{
    Q_OBJECT

public:
    explicit PluginImportExport();
    ~PluginImportExport();
    
    typedef QMap<int, QObject *> Embeds;

    // menu for list N
    virtual bool embed(int list, QMenu *menu) = 0;

    virtual QList<Hotkey> supportedHotkeysInList() const
    {
        return QList<Hotkey>();
    }

    virtual void listHotkeyActivated(int list, const Hotkey &ke)
    {
        Q_UNUSED(list)
        Q_UNUSED(ke)
    }

    // used by ExportTickersTo plugins
    virtual bool exportTickers(const QStringList &tickers)
    {
        Q_UNUSED(tickers)
        return true;
    }

protected:
    int senderStandaloneActionToList() const;
    int senderMenuActionToList() const;

    Embeds &embeds() const;

signals:
    // used by AddTickersFrom plugins
    void sendTickers(int list, const QStringList &tickers);

    // used by ExportTickersTo plugins
    void requestTickers(int list);

private:
    PluginImportExportPrivate *d;
};

#endif // PLUGINIMPORTEXPORT_H
