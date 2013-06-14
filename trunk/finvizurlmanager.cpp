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

#include "finvizaccesssetup.h"
#include "finvizurlmanager.h"
#include "finvizurl.h"
#include "settings.h"

#include "ui_datamanagerbase.h"

FinvizUrlManager::FinvizUrlManager(QWidget *parent) :
    DataManagerBase(parent)
{
    setWindowTitle(tr("Finviz links"));

    d->ui->pushAdd->setText(tr("Add"));
    d->ui->tree->headerItem()->setText(1, tr("Link"));
    d->ui->tree->setWhatsThis(QString("<a href=\"http://www.youtube.com/watch?v=r1Y7iNM7_9k\">%1</a>").arg(tr("Open YouTube tutorial")));

    const QList<FinvizUrl> urls = SETTINGS_GET_FINVIZ_URLS(SETTING_FINVIZ_URLS);

    foreach(FinvizUrl fu, urls)
    {
        addFinvizUrl(fu);
    }

    d->ui->tree->setCurrentItem(d->ui->tree->topLevelItem(0));

    connect(d->ui->tree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotCheckItem(QTreeWidgetItem*,int)));

    //: noun
    QPushButton *b = new QPushButton(tr("Access..."), this);
    connect(b, SIGNAL(clicked()), this, SLOT(slotAccessClicked()));
    addButton(b);
}

FinvizUrlManager::~FinvizUrlManager()
{}

QList<FinvizUrl> FinvizUrlManager::urls() const
{
    QList<FinvizUrl> urls;
    QTreeWidgetItem *i = d->ui->tree->topLevelItem(0);

    while(i)
    {
        urls.append(FinvizUrl(i->text(0), QUrl::fromUserInput(i->text(1))));
        i = d->ui->tree->itemBelow(i);
    }

    return urls;
}

void FinvizUrlManager::addFinvizUrl(const FinvizUrl &fu, bool edit)
{
    addItem(QStringList() << fu.name << fu.url.toString(), QVariant(), edit);
}

void FinvizUrlManager::slotAdd()
{
    addFinvizUrl(FinvizUrl(tr("Tickers"), tr("<Paste url here>")), true);
    d->changed = true;
}

void FinvizUrlManager::slotCheckItem(QTreeWidgetItem *i, int column)
{
    if(!i || column != 1)
        return;

    // check host
    QUrl u = QUrl::fromUserInput(i->text(1));
    QString h = u.host().toLower();

    if(h != FINVIZ && h != FINVIZ_ELITE)
        i->setText(1, tr("<Paste url here>"));
    else
        i->setText(1, u.toString());
}

void FinvizUrlManager::slotAccessClicked()
{
    FinvizAccessSetup fas(this);

    if(fas.exec() == QDialog::Accepted)
    {
        SETTINGS_SET_STRING(SETTING_FINVIZ_EMAIL, fas.email(), Settings::NoSync);
        SETTINGS_SET_STRING(SETTING_FINVIZ_PASSWORD, fas.password());
    }
}
