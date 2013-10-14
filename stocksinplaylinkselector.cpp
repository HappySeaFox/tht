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

#include <QListWidgetItem>

#include "stocksinplaylinkselector.h"
#include "stocksinplayurl.h"
#include "settings.h"
#include "tools.h"
#include "ui_stocksinplaylinkselector.h"

StocksInPlayLinkSelector::StocksInPlayLinkSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StocksInPlayLinkSelector)
{
    ui->setupUi(this);

    ui->pushAdd->setText(Tools::addTitle());

    const QList<StocksInPlayUrl> urls = SETTINGS_GET_STOCKSINPLAY_URLS(SETTING_STOCKSINPLAY_URLS);

    foreach(StocksInPlayUrl su, urls)
    {
        QListWidgetItem *i = new QListWidgetItem(su.name, ui->listWidget);
        i->setData(Qt::UserRole, su.url);
        ui->listWidget->addItem(i);
    }

    ui->listWidget->setCurrentRow(0, QItemSelectionModel::ClearAndSelect);
}

StocksInPlayLinkSelector::~StocksInPlayLinkSelector()
{
    delete ui;
}

void StocksInPlayLinkSelector::slotAdd()
{
    QListWidgetItem *i = ui->listWidget->currentItem();

    m_url = i ? i->data(Qt::UserRole).toUrl() : QUrl();

    accept();
}
