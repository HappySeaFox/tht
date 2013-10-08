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

#include <QUrl>

#include "stocksinplayaccesssetup.h"
#include "stocksinplayurlmanager.h"
#include "stocksinplayurl.h"
#include "settings.h"
#include "tools.h"

#include "ui_datamanagerbase.h"

StocksInPlayUrlManager::StocksInPlayUrlManager(QWidget *parent) :
    DataManagerBase(parent)
{
    //: Stocks In Play - Stock Screener, http://stocksinplay.ru. It's ok not to translate "Stocks In Play" (e.g. you can just copy-paste "Stocks In Play" to your translation). Means "Links to Stocks In Play"
    setWindowTitle(tr("Stocks In Play links"));

    //: This is the label on a button that user pushes to issue the command
    buttonAdd()->setText(tr("Add"));

    QTreeWidget *t = tree();

    //: Noun
    t->headerItem()->setText(1, tr("Link"));
#warning TODO link
    t->setWhatsThis(QString("<a href=\"http://www.youtube.com/watch?v=\">%1</a>").arg(Tools::openYoutubeTutorialTitle()));

    const QList<StocksInPlayUrl> urls = SETTINGS_GET_STOCKSINPLAY_URLS(SETTING_STOCKSINPLAY_URLS);

    foreach(StocksInPlayUrl su, urls)
    {
        addStocksInPlayUrl(su);
    }

    t->setCurrentItem(t->topLevelItem(0));

    connect(t, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotCheckItem(QTreeWidgetItem*,int)));

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
        urls.append(StocksInPlayUrl(i->text(0), QUrl::fromUserInput(i->text(1))));
        i = t->itemBelow(i);
    }

    return urls;
}

void StocksInPlayUrlManager::addStocksInPlayUrl(const StocksInPlayUrl &su, bool edit)
{
    addItem(QStringList() << su.name << su.url.toString(), QVariant(), edit);
}

void StocksInPlayUrlManager::slotAdd()
{
    addStocksInPlayUrl(StocksInPlayUrl(tr("Tickers"),
                           //: Command addressed to the user. Means "Please paste some URL here". "URL" can be translated as "link"
                           tr("<Paste url here>")),
                 true);
    setChanged(true);
}

void StocksInPlayUrlManager::slotCheckItem(QTreeWidgetItem *i, int column)
{
    if(!i || column != 1)
        return;

    // check host
    QUrl u = QUrl::fromUserInput(i->text(1));
    QString h = u.host().toLower();

    if(h != STOCKSINPLAY)
        i->setText(1, tr("<Paste url here>"));
    else
        i->setText(1, u.toString());
}

void StocksInPlayUrlManager::slotAccessClicked()
{
    StocksInPlayAccessSetup sip(this);

    if(sip.exec() == QDialog::Accepted)
    {
        SETTINGS_SET_STRING(SETTING_STOCKSINPLAY_ID, sip.id(), Settings::NoSync);
        SETTINGS_SET_STRING(SETTING_STOCKSINPLAY_HASH, sip.hash());
    }
}
