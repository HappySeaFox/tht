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

#include "updatechecker.h"
#include "about.h"
#include "ui_about.h"

About::About(const QString &newVersion, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->labelVersion->setText(QString("THT %1").arg(NVER_STRING));

    slotNewVersion(newVersion);

    connect(UpdateChecker::instance(), SIGNAL(newVersion(const QString &)), this, SLOT(slotNewVersion(const QString &)));
}

About::~About()
{
    delete ui;
}

void About::slotNewVersion(const QString &newVersion)
{
    ui->labelUpdate->setVisible(!newVersion.isEmpty());
}
