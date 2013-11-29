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

#include <QApplication>
#include <QImageWriter>
#include <QKeySequence>
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QByteArray>
#include <QDateTime>
#include <QFileInfo>
#include <QPixmap>
#include <QBuffer>
#include <QList>
#include <QDir>

#include "screenshoteditor.h"
#include "dropboxuploader.h"
#include "savescreenshot.h"
#include "thtsettings.h"
#include "remotedate.h"
#include "settings.h"
#include "tools.h"

#include "ui_savescreenshot.h"

SaveScreenshot::SaveScreenshot(const QPixmap &px, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveScreenshot)
{
    ui->setupUi(this);

    ui->pushDropbox->setWhatsThis(QString("<a href=\"http://www.youtube.com/watch?v=KcHkzeHIjjw\">%1</a>").arg(Tools::openYoutubeTutorialTitle()));

    // some hotkeys
    ui->pushEdit->setShortcut(Qt::Key_E);
    ui->pushClipboard->setShortcut(Qt::Key_C);
    ui->pushFile->setShortcut(Qt::Key_F);
    ui->pushDropbox->setShortcut(Qt::Key_D);

    m_editor = new ScreenshotEditor(px, this);

    connect(ui->pushEdit, SIGNAL(clicked()), m_editor, SLOT(exec()));
}

SaveScreenshot::~SaveScreenshot()
{
    delete m_editor;
    delete ui;
}

QString SaveScreenshot::baseFileName(bool shortOne) const
{
    QDateTime date = RemoteDate("Eastern Standard Time").dateTime();

    if(!date.isValid())
        date = QDateTime::currentDateTime();

    return date.toString(shortOne ? "yyyy-MM-dd" : "yyyy-MM-dd hh:mm:ss");
}

void SaveScreenshot::slotClipboard()
{
    QApplication::clipboard()->setPixmap(m_editor->renderPixmap());
    accept();
}

void SaveScreenshot::slotFile()
{
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

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"),
                                                    SETTINGS_GET_STRING(SETTING_LAST_SCREENSHOT_DIRECTORY)
                                                    + QDir::separator()
                                                    + baseFileName(true)
                                                    + '-',
                                                    filter, &selectedFilter);

    if(fileName.isEmpty())
        return;

    SETTINGS_SET_STRING(SETTING_LAST_SCREENSHOT_DIRECTORY, QFileInfo(fileName).absolutePath());

    if(m_editor->renderPixmap().save(fileName))
    {
        qDebug("Screenshot has been saved to \"%s\"", qPrintable(fileName));
        accept();
    }
    else
    {
        QMessageBox::critical(this,
                              Tools::errorTitle(),
                              //: Message displayed to the user
                              tr("Cannot save screenshot"));
        qDebug("Cannot save screenshot");
    }
}

void SaveScreenshot::slotDropbox()
{
    QByteArray binary;
    QBuffer buffer(&binary);

    if(buffer.open(QIODevice::WriteOnly) && m_editor->renderPixmap().save(&buffer, "PNG"))
    {
        DropBoxUploader u(baseFileName(false) + ".png", binary, this);

        u.exec();

        switch(u.status())
        {
            case DropBoxUploader::Done:
                qDebug("Screenshot has been saved to Dropbox");
                accept();
            break;

            case DropBoxUploader::Error:
                QMessageBox::critical(this, Tools::errorTitle(), tr("Cannot save screenshot"));
                qDebug("Cannot save screenshot");
            break;

            case DropBoxUploader::Break:
            case DropBoxUploader::NeedRestart:
            break;
        }
    }
}
