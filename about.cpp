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

#include <QKeySequence>
#include <QShortcut>

#include "updatechecker.h"
#include "settings.h"
#include "about.h"

#include "ui_about.h"

About::About(const QString &newVersion, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About),
    m_showExt(true)
{
    ui->setupUi(this);
    ui->labelVersion->setText(QString("THT %1").arg(NVER_STRING));

    slotNewVersion(newVersion);

    connect(UpdateChecker::instance(), SIGNAL(newVersion(const QString &)), this, SLOT(slotNewVersion(const QString &)));

    QLabel *label = new QLabel(QString(
                                   "<table align=center>"
                                   "<tr><td><b>%1: </b></td><td>%2</td></tr>"
                                   "<tr><td><b>%3: </b></td><td>%4</td></tr>"
                                   "</table>"
                                   )
                               .arg(tr("Persistent database"))
                               .arg(Settings::instance()->persistentDatabaseTimestamp().toString(Settings::instance()->databaseTimestampFormat()))
                               .arg(tr("Downloadable database"))
                               .arg(Settings::instance()->mutableDatabaseTimestamp().toString(Settings::instance()->databaseTimestampFormat()))
                               );

    label->setContentsMargins(0, 0, 0, 12);

    setOrientation(Qt::Vertical);
    setExtension(label);

    new QShortcut(QKeySequence::HelpContents, this, SLOT(slotExtendedAbout()));
}

About::~About()
{
    delete ui;
}

void About::slotNewVersion(const QString &newVersion)
{
    ui->labelUpdate->setVisible(!newVersion.isEmpty());
}

void About::slotExtendedAbout()
{
    showExtension(m_showExt);
    m_showExt = !m_showExt;
}
