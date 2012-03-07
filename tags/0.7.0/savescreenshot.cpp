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

#include <QImageWriter>
#include <QFileDialog>
#include <QFileInfo>

#include "settings.h"
#include "savescreenshot.h"
#include "ui_savescreenshot.h"

SaveScreenshot::SaveScreenshot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveScreenshot)
{
    ui->setupUi(this);
}

SaveScreenshot::~SaveScreenshot()
{
    delete ui;
}

void SaveScreenshot::slotClipboard()
{
    m_dest = SaveScreenshotToClipboard;
    accept();
}

void SaveScreenshot::slotFile()
{
    m_dest = SaveScreenshotToFile;

    QString filter;
    QList<QByteArray> formats = QImageWriter::supportedImageFormats();
    QString selectedFilter, current;

    foreach(QByteArray ba, formats)
    {
        current = tr("Image %1 (*.%2)").arg(ba.toUpper().constData()).arg(ba.toLower().constData());

        if(selectedFilter.isEmpty() && ba.toLower() == "png")
            selectedFilter = current;

        filter += current + ";;";
    }

    m_fileName = QFileDialog::getSaveFileName(this, tr("Save as"), Settings::instance()->lastScreenShotDirectory(), filter, &selectedFilter);

    if(m_fileName.isEmpty())
        reject();
    else
    {
        Settings::instance()->setLastScreenShotDirectory(QFileInfo(m_fileName).canonicalPath());
        accept();
    }
}
