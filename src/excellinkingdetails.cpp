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

#include <QRegExpValidator>

#include "excellinkingdetails.h"
#include "tools.h"
#include "ui_excellinkingdetails.h"

ExcelLinkingDetails::ExcelLinkingDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExcelLinkingDetails)
{
    ui->setupUi(this);

    ui->lineCell->setValidator(new QRegExpValidator(Tools::cellValidator(), ui->lineCell));
}

ExcelLinkingDetails::~ExcelLinkingDetails()
{
    delete ui;
}

QString ExcelLinkingDetails::book() const
{
    return ui->lineBook->text();
}

QString ExcelLinkingDetails::sheet() const
{
    return ui->lineSheet->text();
}

QString ExcelLinkingDetails::cell() const
{
    return ui->lineCell->text();
}
