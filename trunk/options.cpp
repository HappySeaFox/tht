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

#include "settings.h"
#include "options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);

    for(int i = 0;i < Settings::instance()->maximumNumberOfLists();i++)
        ui->comboNumberOfLists->addItem(QString::number(i+1));

    load();
}

Options::~Options()
{
    delete ui;
}

void Options::load()
{
    ui->comboNumberOfLists->setCurrentIndex(Settings::instance()->numberOfLists() - 1);
    ui->checkOnTop->setChecked(Settings::instance()->onTop());
    ui->checkTray->setChecked(Settings::instance()->hideToTray());
    ui->checkSave->setChecked(Settings::instance()->saveGeometry());
    ui->checkSaveTickers->setChecked(Settings::instance()->saveTickers());
}

void Options::save()
{
    Settings::instance()->setNumberOfLists(ui->comboNumberOfLists->currentIndex()+1);
    Settings::instance()->setOnTop(ui->checkOnTop->isChecked());
    Settings::instance()->setHideToTray(ui->checkTray->isChecked());
    Settings::instance()->setSaveGeometry(ui->checkSave->isChecked());
    Settings::instance()->setSaveTickers(ui->checkSaveTickers->isChecked());
}
