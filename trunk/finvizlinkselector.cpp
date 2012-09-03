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

#include "finvizlinkselector.h"
#include "settings.h"
#include "ui_finvizlinkselector.h"

FinvizLinkSelector::FinvizLinkSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FinvizLinkSelector)
{
    ui->setupUi(this);

    m_urls = Settings::instance()->finvizUrls();

    foreach(FinvizUrl fu, m_urls)
    {
        ui->listWidget->addItem(fu.name);
    }

    ui->listWidget->setCurrentRow(0, QItemSelectionModel::ClearAndSelect);
}

FinvizLinkSelector::~FinvizLinkSelector()
{
    delete ui;
}

void FinvizLinkSelector::slotAdd()
{
    QString current = ui->listWidget->currentItem()->text();

    if(!current.isEmpty())
    {
        foreach(FinvizUrl fu, m_urls)
        {
            if(fu.name == current)
            {
                m_url = fu.url;
                break;
            }
        }
    }

    accept();
}
