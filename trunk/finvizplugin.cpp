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

#include "finvizlinkselector.h"
#include "finvizurlmanager.h"
#include "finvizdownloader.h"
#include "finvizplugin.h"
#include "finvizurl.h"
#include "settings.h"

// serialize/deserialize FinvizUrl
static QDataStream &operator<<(QDataStream &out, const FinvizUrl &fu)
{
    out << fu.name << fu.url;
    return out;
}

static QDataStream &operator>>(QDataStream &in, FinvizUrl &fu)
{
    in >> fu.name >> fu.url;
    return in;
}

FinvizPlugin::FinvizPlugin() :
    PluginImportExport()
{
    THT_PLUGIN_INTERFACE_IMPLEMENTATION

    qRegisterMetaTypeStreamOperators<FinvizUrl>("FinvizUrl");
    qRegisterMetaTypeStreamOperators<QList<FinvizUrl> >("QList<FinvizUrl>");
}

bool FinvizPlugin::init()
{
#define FINVIZ_URL "http://" FINVIZ "/screener.ashx?v=411&"

    // default Finviz urls
    if(!Settings::instance()->contains("settings/" SETTING_FINVIZ_URLS))
    {
        SETTINGS_SET_FINVIZ_URLS(SETTING_FINVIZ_URLS,
                      QList<FinvizUrl>()
                      << FinvizUrl("NYSE >1$ >300k By Ticker",              QUrl(FINVIZ_URL "f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=ticker"))
                      << FinvizUrl("NYSE >1$ >300k By Change From Open",    QUrl(FINVIZ_URL "f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=-changeopen"))
                      << FinvizUrl("NYSE >1$ >300k Most Active From Open",  QUrl(FINVIZ_URL "s=ta_mostactive&f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=-changeopen"))
                      << FinvizUrl("NYSE >1$ >300k Top Gainers",            QUrl(FINVIZ_URL "s=ta_topgainers&f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1"))
                      << FinvizUrl("NYSE >1$ >300k Top Losers",             QUrl(FINVIZ_URL "s=ta_toplosers&f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1"))
                      << FinvizUrl("NYSE >1$ >300k New High",               QUrl(FINVIZ_URL "s=ta_newhigh&f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=-change"))
                      << FinvizUrl("NYSE >1$ >300k New Low",                QUrl(FINVIZ_URL "s=ta_newlow&f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=-change"))
                      << FinvizUrl("NYSE >1$ >300k Volume>1.5",             QUrl(FINVIZ_URL "f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1,sh_relvol_o1.5&o=-change"))
                      << FinvizUrl("NYSE >1$ >300k Average True Range>1",   QUrl(FINVIZ_URL "f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1,ta_averagetruerange_o1&ft=3&o=-change"))
                      << FinvizUrl("NYSE >1$ >300k Earn Yest After Close",  QUrl(FINVIZ_URL "f=earningsdate_yesterdayafter,exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=-change"))
                      << FinvizUrl("NYSE >1$ >300k Earn Today Before Open", QUrl(FINVIZ_URL "f=earningsdate_todaybefore,exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=-change"))
                      );
    }

#undef FINVIZ_URL

    return true;
}

bool FinvizPlugin::embed(int list, QMenu *parentMenu)
{
    PluginImportExport::Embeds &emb = embeds();

    if(emb.contains(list))
        return true;

    //: This is the label on a menu item that user clicks to issue the command. Finviz - Stock Screener, http://finviz.com. It's ok not to translate "Finviz" (e.g. you can just copy-paste "Finviz" to your translation)
    QMenu *menu = new QMenu(tr("Add from Finviz") + "\tZ");

    if(!menu)
        return false;

    menu->setIcon(QIcon(":/images/finviz.png"));

    rebuildMenu(menu);

    parentMenu->addMenu(menu);

    emb.insert(list, menu);

    return true;
}

void FinvizPlugin::listHotkeyActivated(int list, const Hotkey &h)
{
    if(h == Hotkey(Qt::Key_Z))
    {
        showFinvizSelector(list);
    }
}

void FinvizPlugin::addFromFinviz(int list, const QUrl &u)
{
    FinvizDownloader dn(u, topLevelWidget());

    if(dn.exec() != QDialog::Accepted)
        return;

    emit sendTickers(list, dn.tickers());
}

void FinvizPlugin::showFinvizSelector(int list)
{
    FinvizLinkSelector ls(topLevelWidget());

    if(ls.exec() != QDialog::Accepted)
        return;

    addFromFinviz(list, ls.url());
}

void FinvizPlugin::rebuildMenu(QMenu *menu)
{
    if(!menu)
        return;

    menu->clear();

    QList<FinvizUrl> urls = SETTINGS_GET_FINVIZ_URLS(SETTING_FINVIZ_URLS);

    foreach(FinvizUrl fu, urls)
    {
        QAction *a = menu->addAction(fu.name, this, SLOT(slotAdd()));
        a->setData(fu.url);
    }

    if(!urls.isEmpty())
        menu->addSeparator();

    menu->addAction(QIcon(":/images/finviz-customize.png"), tr("Customize..."), this, SLOT(slotManageUrls()));
}

void FinvizPlugin::slotAdd()
{
    qDebug("Add from Finviz");

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

    addFromFinviz(list, u);
}

void FinvizPlugin::slotManageUrls()
{
    FinvizUrlManager mgr(topLevelWidget());

    if(mgr.exec() == QDialog::Accepted && mgr.changed())
    {
        SETTINGS_SET_FINVIZ_URLS(SETTING_FINVIZ_URLS, mgr.urls());
        const PluginImportExport::Embeds &emb = embeds();

        Embeds::const_iterator itEnd = emb.end();

        for(Embeds::const_iterator it = emb.begin();it != itEnd;++it)
        {
            rebuildMenu(qobject_cast<QMenu *>(it.value()));
        }
    }
}

PLUGIN_CONSTRUCTOR(FinvizPlugin)
