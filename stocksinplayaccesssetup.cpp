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

#include <QStyle>
#include <QIcon>

#include "stocksinplayaccesssetup.h"
#include "stocksinplayurl.h"
#include "settings.h"
#include "ui_stocksinplayaccesssetup.h"

StocksInPlayAccessSetup::StocksInPlayAccessSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StocksInPlayAccessSetup)
{
    ui->setupUi(this);

    ui->labelNotePixmap->setPixmap(style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(16, 16));

    ui->lineId->setText(SETTINGS_GET_STRING(SETTING_STOCKSINPLAY_ID));
    ui->lineHash->setText(SETTINGS_GET_STRING(SETTING_STOCKSINPLAY_HASH));
}

StocksInPlayAccessSetup::~StocksInPlayAccessSetup()
{
    delete ui;
}

QString StocksInPlayAccessSetup::id() const
{
    return ui->lineId->text();
}

QString StocksInPlayAccessSetup::hash() const
{
    return ui->lineHash->text();
}
