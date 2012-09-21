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

#include <QDesktopWidget>
#include <QApplication>
#include <QKeySequence>
#include <QScrollBar>
#include <QKeyEvent>
#include <QPixmap>

#include "colorpickerwidget.h"
#include "screenshoteditor.h"
#include "selectablelabel.h"
#include "settings.h"
#include "ui_screenshoteditor.h"

ScreenshotEditor::ScreenshotEditor(const QPixmap &px, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreenshotEditor)
{
    ui->setupUi(this);

    ui->scrollAreaWidgetContents->setPixmap(px);
    ui->scrollArea->setWhatsThis(QString("<a href=\"http://www.youtube.com/watch?v=EpVSUyPC3WA\">%1</a>").arg(tr("Open YouTube tutorial")));

    connect(ui->pushText, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startText()));
    connect(ui->pushLong, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startBuy()));
    connect(ui->pushShort, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startSell()));
    connect(ui->pushStop, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startStop()));
    connect(ui->pushEllipse, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startEllipse()));
    connect(ui->pushDelete, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(deleteSelected()));
    connect(ui->pushSelectAll, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(selectAll()));

    connect(ui->scrollAreaWidgetContents, SIGNAL(selected(SelectableLabel*,bool)), this, SLOT(slotSelected(SelectableLabel*,bool)));

    ui->pushDelete->setShortcut(QKeySequence::Delete);
    ui->pushSelectAll->setShortcut(QKeySequence::SelectAll);

    layout()->activate();

    const QRect rc = QDesktopWidget().availableGeometry(window());

    const int maxw = rc.width() * 0.75;
    const int maxh = rc.height() * 0.75;

    int neww = width() + (px.width() - ui->scrollArea->width() + 4);
    int newh = height() + (px.height() - ui->scrollArea->height() + 4);

    resize(qBound(width(), neww, maxw), qBound(height(), newh, maxh));
}

ScreenshotEditor::~ScreenshotEditor()
{
    delete ui;
}

QPixmap ScreenshotEditor::pixmap()
{
    return ui->scrollAreaWidgetContents->pixmap();
}

int ScreenshotEditor::exec()
{
    ui->scrollAreaWidgetContents->restoreLabels();

    // colors
    setEllipseBorderColor(Settings::instance()->ellipseBorderColor());
    setEllipseFillColor(Settings::instance()->ellipseFillColor());

    m_oellipseBorderColor = m_ellipseBorderColor;
    m_oellipseFillColor = m_ellipseFillColor;

    int code = QDialog::exec();

    if(code == QDialog::Accepted)
        ui->scrollAreaWidgetContents->saveLabels();
    else
    {
        ui->scrollAreaWidgetContents->clearLabels();

        Settings::instance()->setEllipseBorderColor(m_oellipseBorderColor, Settings::NoSync);
        Settings::instance()->setEllipseFillColor(m_oellipseFillColor);
    }

    return code;
}

void ScreenshotEditor::keyPressEvent(QKeyEvent *ke)
{
    if(ke->key() == Qt::Key_Escape
            && ui->scrollAreaWidgetContents->editType() != ScreenshotEditorWidget::None)
    {
        ui->scrollAreaWidgetContents->cancel();
        return;
    }

    QDialog::keyPressEvent(ke);
}

void ScreenshotEditor::setEllipseBorderColor(const QColor &c)
{
    m_ellipseBorderColor = c;

    QPixmap px(16, 16);
    px.fill(c);

    ui->pushEllipseBorder->setIcon(px);
}

void ScreenshotEditor::setEllipseFillColor(const QColor &c)
{
    m_ellipseFillColor = c;
    m_ellipseFillColor.setAlpha(20);

    QColor cp = c;
    cp.setAlpha(255);

    QPixmap px(16, 16);
    px.fill(cp);

    ui->pushEllipseFill->setIcon(px);
}

void ScreenshotEditor::slotSelected(SelectableLabel *sl, bool selected)
{
    if(!selected)
        return;

    const QList<SelectableLabel *> list = ui->scrollAreaWidgetContents->labels();

    if(QApplication::keyboardModifiers() != Qt::ControlModifier)
    {
        foreach(SelectableLabel *l, list)
        {
            if(l != sl)
                l->setSelected(false);
        }
    }
}

void ScreenshotEditor::slotEllipseBorderColor()
{
    ColorPickerWidget cpw(this);

    cpw.setColor(m_ellipseBorderColor);

    if(cpw.exec() == QDialog::Accepted)
    {
        setEllipseBorderColor(cpw.color());
        Settings::instance()->setEllipseBorderColor(m_ellipseBorderColor);
    }
}

void ScreenshotEditor::slotEllipseFillColor()
{
    ColorPickerWidget cpw(this);

    QColor c = m_ellipseFillColor;
    c.setAlpha(255);

    cpw.setColor(c);

    if(cpw.exec() == QDialog::Accepted)
    {
        setEllipseFillColor(cpw.color());
        Settings::instance()->setEllipseFillColor(m_ellipseFillColor);
    }
}