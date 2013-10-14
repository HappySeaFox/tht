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

#include "finvizlinkselector.h"
#include "finvizurl.h"
#include "settings.h"
#include "tools.h"
#include "ui_finvizlinkselector.h"

FinvizLinkSelector::FinvizLinkSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FinvizLinkSelector)
{
    ui->setupUi(this);

    ui->pushAdd->setText(Tools::addTitle());

    const QList<FinvizUrl> urls = SETTINGS_GET_FINVIZ_URLS(SETTING_FINVIZ_URLS);

    foreach(FinvizUrl fu, urls)
    {
        QListWidgetItem *i = new QListWidgetItem(fu.name, ui->listWidget);
        i->setData(Qt::UserRole, fu.url);
        ui->listWidget->addItem(i);
    }

    ui->listWidget->setCurrentRow(0, QItemSelectionModel::ClearAndSelect);
}

FinvizLinkSelector::~FinvizLinkSelector()
{
    delete ui;
}

void FinvizLinkSelector::slotAdd()
{
    QListWidgetItem *i = ui->listWidget->currentItem();

    m_url = i ? i->data(Qt::UserRole).toUrl() : QUrl();

    accept();
}
