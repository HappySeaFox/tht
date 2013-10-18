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
#include <QUrl>

#include "stocksinplaylinkselector.h"
#include "stocksinplayurlmanager.h"
#include "stocksinplaydownloader.h"
#include "stocksinplayplugin.h"
#include "stocksinplayurl.h"
#include "settings.h"
#include "tools.h"

// serialize/deserialize StocksInPlayUrl
static QDataStream &operator<<(QDataStream &out, const StocksInPlayUrl &fu)
{
    out << fu.name << fu.url;
    return out;
}

static QDataStream &operator>>(QDataStream &in, StocksInPlayUrl &fu)
{
    in >> fu.name >> fu.url;
    return in;
}

StocksInPlayPlugin::StocksInPlayPlugin() :
    PluginImportExport()
{
    THT_PLUGIN_INTERFACE_IMPLEMENTATION

    qRegisterMetaTypeStreamOperators<StocksInPlayUrl>("StocksInPlayUrl");
    qRegisterMetaTypeStreamOperators<QList<StocksInPlayUrl> >("QList<StocksInPlayUrl>");
}

bool StocksInPlayPlugin::init()
{
#define STOCKSINPLAY_URL "http://" STOCKSINPLAY "/basic.php?"

    // default StocksInPlay urls
    if(!Settings::instance()->contains("settings/" SETTING_STOCKSINPLAY_URLS))
    {
        SETTINGS_SET_STOCKSINPLAY_URLS(SETTING_STOCKSINPLAY_URLS,
                      QList<StocksInPlayUrl>()
                      << StocksInPlayUrl("NYSE >1$ >300k By Ticker",              QUrl(STOCKSINPLAY_URL "min_price=1&min_avgvol=300&Exchange=1&order=ticker&asc=1&row=all&"))
                      << StocksInPlayUrl("NYSE >1$ >300k New High",               QUrl(STOCKSINPLAY_URL "min_price=1&min_avgvol=300&Exchange=1&signal=1&order=chpo&row=all&"))
                      << StocksInPlayUrl("NYSE >1$ >300k New Low",                QUrl(STOCKSINPLAY_URL "min_price=1&min_avgvol=300&Exchange=1&signal=2&order=chpo&asc=1&row=all&"))
                      << StocksInPlayUrl("NYSE >1$ >300k Volume>1.5",             QUrl(STOCKSINPLAY_URL "min_price=1&min_avgvol=300&min_revol=1.5&Exchange=1&order=ticker&asc=1&row=all&"))
                      << StocksInPlayUrl("NYSE >1$ >300k Average True Range>1",   QUrl(STOCKSINPLAY_URL "min_price=1&min_avgvol=300&min_atr=>1&Exchange=1&order=ticker&asc=1&row=all&"))
                      );
    }

#undef STOCKSINPLAY_URL

    return true;
}

bool StocksInPlayPlugin::embed(int list, QMenu *parentMenu)
{
    PluginImportExport::Embeds &emb = embeds();

    if(emb.contains(list))
        return true;

    //: This is the label on a menu item that user clicks to issue the command. Stocks In Play - Stock Screener, http://stocksinplay.ru. It's ok not to translate "Stocks In Play" (e.g. you can just copy-paste "Stocks In Play" to your translation)
    QMenu *menu = new QMenu(tr("Add from Stocks In Play") + "\tV");

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

void StocksInPlayPlugin::addFromStocksInPlay(int list, const QUrl &u)
{
    StocksInPlayDownloader dn(u, topLevelWidget());

    if(dn.exec() != QDialog::Accepted)
        return;

    emit sendTickers(list, dn.tickers());
}

void StocksInPlayPlugin::showStocksInPlaySelector(int list)
{
    StocksInPlayLinkSelector ls(topLevelWidget());

    if(ls.exec() != QDialog::Accepted)
        return;

    addFromStocksInPlay(list, ls.url());
}

void StocksInPlayPlugin::rebuildMenu(QMenu *menu)
{
    if(!menu)
        return;

    menu->clear();

    QList<StocksInPlayUrl> urls = SETTINGS_GET_STOCKSINPLAY_URLS(SETTING_STOCKSINPLAY_URLS);

    foreach(StocksInPlayUrl fu, urls)
    {
        QAction *a = menu->addAction(fu.name, this, SLOT(slotAdd()));
        a->setData(fu.url);
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

    QUrl u = a->data().toUrl();

    if(!u.isValid())
    {
        qDebug("Url \"%s\" is not valid", qPrintable(u.toString(QUrl::RemovePassword)));
        return;
    }

    addFromStocksInPlay(list, u);
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
