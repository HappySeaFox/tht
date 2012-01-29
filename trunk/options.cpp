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

#include <QSettings>

#include "options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);

    load();
}

Options::~Options()
{
    delete ui;
}

void Options::load()
{
    QSettings settings;
    bool ok;

    settings.beginGroup("settings");
    uint lists = settings.value("number-of-lists", 3).toUInt(&ok);

    if(!ok)
        lists = 3;

    if(lists < 1 || lists > 5)
        lists = 3;

    ui->comboNumberOfLists->setCurrentIndex(lists - 1);
    settings.endGroup();
}

void Options::save()
{
    QSettings settings;

    settings.beginGroup("settings");
    settings.setValue("number-of-lists", ui->comboNumberOfLists->currentIndex()+1);
    settings.endGroup();
}
