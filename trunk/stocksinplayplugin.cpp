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

#include <QDataStream>
#include <QAction>
#include <QMenu>

#include "stocksinplaylinkselector.h"
#include "stocksinplayurlmanager.h"
#include "stocksinplaydownloader.h"
#include "stocksinplayplugin.h"
#include "stocksinplaytools.h"
#include "stocksinplayurl.h"
#include "settings.h"
#include "tools.h"

StocksInPlayPlugin::StocksInPlayPlugin() :
    PluginImportExport()
{
    THT_PLUGIN_INTERFACE_IMPLEMENTATION

    qRegisterMetaTypeStreamOperators<StocksInPlayUrl>("StocksInPlayUrl");
    qRegisterMetaTypeStreamOperators<QList<StocksInPlayUrl> >("QList<StocksInPlayUrl>");
}

bool StocksInPlayPlugin::init()
{
    // default StocksInPlay urls
    if(!Settings::instance()->contains("/" SETTING_STOCKSINPLAY_URLS))
    {
        SETTINGS_SET_STOCKSINPLAY_URLS(SETTING_STOCKSINPLAY_URLS,
                      QList<StocksInPlayUrl>()
                        << StocksInPlayUrl("NYSE >1$ >300k By Ticker",            "U0VMRUNUICogRlJPTSBxX2JhemEgTEVGVCBKT0lOIHFfbGV2ZWwxIFVTSU5HICh0aWNrZXIpIFdIRVJFIHByaWNlPj0nMScgQU5EIGU9JzEnIEFORCBgYXZ2b2A+PTMwMCBPUkRFUiBCWSB0aWNrZXIgQVND")
                        << StocksInPlayUrl("NYSE >1$ >300k New High",             "U0VMRUNUICosIElGKG9wPTAsMCxUUlVOQ0FURSgocHJpY2Uvb3AqMTAwLTEwMCksMikpIGFzIGBjaHBvYCBGUk9NIHFfYmF6YSBMRUZUIEpPSU4gcV9sZXZlbDEgVVNJTkcgKHRpY2tlcikgV0hFUkUgcHJpY2U+PScxJyBBTkQgZT0nMScgQU5EIGBhdnZvYD49MzAwIEFORCAoaGktcHJpY2UpIDw9IDAuMDEgT1JERVIgQlkgY2hwbyBERVND")
                        << StocksInPlayUrl("NYSE >1$ >300k New Low",              "U0VMRUNUICosIElGKG9wPTAsMCxUUlVOQ0FURSgocHJpY2Uvb3AqMTAwLTEwMCksMikpIGFzIGBjaHBvYCBGUk9NIHFfYmF6YSBMRUZUIEpPSU4gcV9sZXZlbDEgVVNJTkcgKHRpY2tlcikgV0hFUkUgcHJpY2U+PScxJyBBTkQgZT0nMScgQU5EIGBhdnZvYD49MzAwIEFORCAocHJpY2UtbG8pIDw9IDAuMDEgT1JERVIgQlkgY2hwbyBBU0M=")
                        << StocksInPlayUrl("NYSE >1$ >300k Volume>1.5",           "U0VMRUNUICosIChJRihhdnZvPTAsMCxUUlVOQ0FURSgodm9sLzEwMDApL2F2dm8sMikpKSBhcyBgcmV2b2xgIEZST00gcV9iYXphIExFRlQgSk9JTiBxX2xldmVsMSBVU0lORyAodGlja2VyKSBXSEVSRSBwcmljZT49JzEnIEFORCBlPScxJyBBTkQgYGF2dm9gPj0zMDAgQU5EICh2b2wvMTAwMCkvYXZ2bz49MS41IE9SREVSIEJZIHRpY2tlciBBU0M=")
                        << StocksInPlayUrl("NYSE >1$ >300k Average True Range>1", "U0VMRUNUICogRlJPTSBxX2JhemEgTEVGVCBKT0lOIHFfbGV2ZWwxIFVTSU5HICh0aWNrZXIpIFdIRVJFIHByaWNlPj0nMScgQU5EIGU9JzEnIEFORCBgYXRyYD49MSBBTkQgYGF2dm9gPj0zMDAgT1JERVIgQlkgdGlja2VyIEFTQw==")
                      );
    }

    StocksInPlayTools::setCachedHash(Tools::decrypt(SETTINGS_GET_BYTE_ARRAY(SETTING_STOCKSINPLAY_HASH)));

    return true;
}

bool StocksInPlayPlugin::embed(int list, QMenu *parentMenu)
{
    PluginImportExport::Embeds &emb = embeds();

    if(emb.contains(list))
        return true;

    QMenu *menu = new QMenu(StocksInPlayTools::addFromStocksInPlayTitle() + "\tV");

    if(!menu)
        return false;

    menu->setIcon(QIcon(":/images/stocksinplay.png"));

    rebuildMenu(menu);

    parentMenu->addMenu(menu);

    emb.insert(list, menu);

    return true;
}

void StocksInPlayPlugin::listHotkeyActivated(int list, const Hotkey &h)
{
    if(h == Hotkey(Qt::Key_V))
    {
        showStocksInPlaySelector(list);
    }
}

void StocksInPlayPlugin::addFromStocksInPlay(int list, const QString &hash)
{
    StocksInPlayDownloader dn(hash, topLevelWidget());

    if(dn.exec() != QDialog::Accepted)
        return;

    emit sendTickers(list, dn.tickers());
}

void StocksInPlayPlugin::showStocksInPlaySelector(int list)
{
    StocksInPlayLinkSelector ls(topLevelWidget());

    if(ls.exec() != QDialog::Accepted || ls.hash().isEmpty())
        return;

    addFromStocksInPlay(list, ls.hash());
}

void StocksInPlayPlugin::rebuildMenu(QMenu *menu)
{
    if(!menu)
        return;

    menu->clear();

    QList<StocksInPlayUrl> urls = SETTINGS_GET_STOCKSINPLAY_URLS(SETTING_STOCKSINPLAY_URLS);

    foreach(StocksInPlayUrl su, urls)
    {
        QAction *a = menu->addAction(su.name, this, SLOT(slotAdd()));
        a->setData(su.hash);
    }

    if(!urls.isEmpty())
        menu->addSeparator();

    menu->addAction(QIcon(":/images/stocksinplay-customize.png"), Tools::customizeTitle() + "...", this, SLOT(slotManageUrls()));
}

void StocksInPlayPlugin::slotAdd()
{
    qDebug("Add from StocksInPlay");

    QAction *a = qobject_cast<QAction *>(sender());
    int list = senderMenuActionToList();

    if(!a || list < 0)
        return;

    QString hash = a->data().toString();

    if(hash.isEmpty())
    {
        qDebug("Hash \"%s\" is empty", qPrintable(hash));
        return;
    }

    addFromStocksInPlay(list, hash);
}

void StocksInPlayPlugin::slotManageUrls()
{
    StocksInPlayUrlManager mgr(topLevelWidget());

    if(mgr.exec() == QDialog::Accepted && mgr.changed())
    {
        SETTINGS_SET_STOCKSINPLAY_URLS(SETTING_STOCKSINPLAY_URLS, mgr.urls());
        const PluginImportExport::Embeds &emb = embeds();

        Embeds::const_iterator itEnd = emb.end();

        for(Embeds::const_iterator it = emb.begin();it != itEnd;++it)
        {
            rebuildMenu(qobject_cast<QMenu *>(it.value()));
        }
    }
}

THT_PLUGIN_CONSTRUCTOR(StocksInPlayPlugin)
