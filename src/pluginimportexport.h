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

#ifndef PLUGINIMPORTEXPORT_H
#define PLUGINIMPORTEXPORT_H

#include <QString>
#include <QList>
#include <QMap>

#include "plugin.h"
#include "hotkey.h"

class QMenu;

class PluginImportExportPrivate;

/*
 *  Base class for 'import' (add tickers) or 'export' (export tickers) plugins
 */
class PluginImportExport : public Plugin
{
    Q_OBJECT

public:
    explicit PluginImportExport();
    ~PluginImportExport();

    /*
     *  "list number => embedded object" mappings
     */
    typedef QMap<int, QObject *> Embeds;

    /*
     *  Method to embed plugin into menus
     */
    virtual bool embed(int list, QMenu *menu) = 0;

    /*
     *  Method to unembed plugin from menus. It deletes the QObject* value
     *  associated with list number in embeds() map
     */
    bool unembed(int list);

    /*
     *  Plugin can support hotkeys. When the user activates
     *  the supported hotkey, THT will call listHotkeyActivated().
     *  Please remember that if the hotkey duplicates the hotkey
     *  from THT itself, this hotkey will be ignored. For example,
     *  if you return Qt::Key_A here, it will be ignored, because
     *  this hotkey is already binded to "Add from file". Returns an empty
     *  list by default
     */
    virtual QList<Hotkey> supportedHotkeysInList() const;

    /*
     *  Supported hotkey has been activated. Plugin must handle it
     */
    virtual void listHotkeyActivated(int list, const Hotkey &ke);

    /*
     *  Export tickers to the destination by the ExportTickersTo plugin
     *
     *  each ticker has the following format:
     *  <ticker name>[,ticker priority][,percent-encoded comment]
     *
     *  for example:
     *  "A"                 - just a ticker "A"
     *  "A,,"               - ticker with default priority and empty comment
     *  "A,0,"              - ticker with priority == default and empty comment
     *  "A,1,cool%20ticker" - ticker with priority == medium and comment "cool ticker"
     *
     *  four priorities are available: 0(default), 1(medium), 2(high), 3(the highest)
     */
    virtual bool exportTickers(const QStringList &tickers);

protected:
    /*
     *  Use this method to determine the list number
     *  where your embedded action is activated. Use this
     *  method only in the slot connected to the appropriate action
     *  and only when you embed only one action into the menu
     */
    int senderStandaloneActionToList() const;

    /*
     *  Use this method to determine the list number
     *  where your embedded submenu's action is activated. Use this
     *  method only in the slot connected to the appropriate action
     *  and only when you embed a submenu into the menu
     *  (refer to the Finviz plugin from the THT sources)
     */
    int senderMenuActionToList() const;

    /*
     *  List of already embedded widgets
     */
    Embeds &embeds() const;

signals:
    /*
     *  Signal to add tickers to the Nth list
     *  from the AddTickersFrom plugin
     */
    void sendTickers(int list, const QStringList &tickers);

    /*
     *  Signal to request tickers from the Nth list by the ExportTickersTo plugin.
     *  Tickers will be sent by the list to the exportTickers() method
     */
    void requestTickers(int list);

private:
    PluginImportExportPrivate *d;
};

#endif // PLUGINIMPORTEXPORT_H
