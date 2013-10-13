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

#include <QPixmap>
#include <QStyle>
#include <QIcon>

#include "finvizaccesssetup.h"
#include "finvizurl.h"
#include "settings.h"
#include "ui_finvizaccesssetup.h"

FinvizAccessSetup::FinvizAccessSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FinvizAccessSetup)
{
    ui->setupUi(this);

    ui->labelSecurityPixmap->setPixmap(style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(16, 16));
    ui->labelNotePixmap->setPixmap(style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(16, 16));

    ui->lineEmail->setText(SETTINGS_GET_STRING(SETTING_FINVIZ_EMAIL));
    ui->linePassword->setText(SETTINGS_GET_STRING(SETTING_FINVIZ_PASSWORD));
}

FinvizAccessSetup::~FinvizAccessSetup()
{
    delete ui;
}

QString FinvizAccessSetup::email() const
{
    return ui->lineEmail->text();
}

QString FinvizAccessSetup::password() const
{
    return ui->linePassword->text();
}
