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

#include "stocksinplayaccesssetup.h"
#include "stocksinplayurlmanager.h"
#include "stocksinplaytools.h"
#include "stocksinplayurl.h"
#include "settings.h"
#include "tools.h"

#include "ui_datamanagerbase.h"

StocksInPlayUrlManager::StocksInPlayUrlManager(QWidget *parent) :
    DataManagerBase(parent)
{
    //: Stocks In Play - Stock Screener, http://stocksinplay.ru. It's ok not to translate "Stocks In Play" (e.g. you can just copy-paste "Stocks In Play" to your translation). Means "Links to Stocks In Play"
    setWindowTitle(tr("Stocks In Play links"));

    buttonAdd()->setText(Tools::addTitle());

    QTreeWidget *t = tree();

    //: Means "Hash key" as in computing
    t->headerItem()->setText(1, tr("Hash"));
    t->setWhatsThis(QString("<a href=\"http://www.youtube.com/watch?v=Daoa0Xftp7M\">%1</a>").arg(Tools::openYoutubeTutorialTitle()));

    const QList<StocksInPlayUrl> urls = SETTINGS_GET_STOCKSINPLAY_URLS(SETTING_STOCKSINPLAY_URLS);

    foreach(StocksInPlayUrl su, urls)
    {
        addStocksInPlayUrl(su);
    }

    t->setCurrentItem(t->topLevelItem(0));

    //: Noun
    QPushButton *b = new QPushButton(tr("Access..."), this);
    connect(b, SIGNAL(clicked()), this, SLOT(slotAccessClicked()));
    addButton(b);
}

StocksInPlayUrlManager::~StocksInPlayUrlManager()
{}

QList<StocksInPlayUrl> StocksInPlayUrlManager::urls() const
{
    QList<StocksInPlayUrl> urls;
    QTreeWidget *t = tree();
    QTreeWidgetItem *i = t->topLevelItem(0);

    while(i)
    {
        urls.append(StocksInPlayUrl(i->text(0), i->text(1)));
        i = t->itemBelow(i);
    }

    return urls;
}

void StocksInPlayUrlManager::addStocksInPlayUrl(const StocksInPlayUrl &su, bool edit)
{
    addItem(QStringList() << su.name << su.hash, QVariant(), edit);
}

void StocksInPlayUrlManager::slotAdd()
{
    addStocksInPlayUrl(StocksInPlayUrl(Tools::tickersTitle(), QString()), true);
    setChanged(true);
}

void StocksInPlayUrlManager::slotAccessClicked()
{
    StocksInPlayAccessSetup sip(this);

    if(sip.exec() == QDialog::Accepted)
    {
        SETTINGS_SET_STRING(SETTING_STOCKSINPLAY_ID, sip.id(), Settings::NoSync);
        SETTINGS_SET_BYTE_ARRAY(SETTING_STOCKSINPLAY_HASH, Tools::encrypt(sip.hash().toUtf8()));
        StocksInPlayTools::setCachedHash(sip.hash());
    }
}
