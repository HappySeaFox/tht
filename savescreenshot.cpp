#include <QImageWriter>
#include <QFileDialog>

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

    m_fileName = QFileDialog::getSaveFileName(this, tr("Save as"), QString(), filter, &selectedFilter);

    accept();
}
