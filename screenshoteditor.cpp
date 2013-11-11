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
#include <QColorDialog>
#include <QScrollBar>
#include <QKeyEvent>
#include <QPixmap>

#include "screenshoteditor.h"
#include "selectablelabel.h"
#include "settings.h"
#include "tools.h"
#include "ui_screenshoteditor.h"

ScreenshotEditor::ScreenshotEditor(const QPixmap &px, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreenshotEditor)
{
    ui->setupUi(this);

    ui->scrollAreaWidgetContents->setPixmap(px);
    ui->scrollArea->setWhatsThis(QString("<a href=\"http://www.youtube.com/watch?v=iE9g_5MvHi4\">%1</a>").arg(Tools::openYoutubeTutorialTitle()));

    connect(ui->pushText,      SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startText()));
    connect(ui->pushLong,      SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startBuy()));
    connect(ui->pushShort,     SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startSale()));
    connect(ui->pushStop,      SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startStop()));
    connect(ui->pushEllipse,   SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startEllipse()));
    connect(ui->pushDelete,    SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(deleteSelected()));
    connect(ui->pushSelectAll, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(selectAll()));

    connect(ui->scrollAreaWidgetContents, SIGNAL(selected(SelectableLabel*,bool)), this, SLOT(slotSelected(SelectableLabel*,bool)));

    // some hotkeys
    ui->pushText->setShortcut(Qt::Key_T);
    ui->pushLong->setShortcut(Qt::Key_B);
    ui->pushShort->setShortcut(Qt::Key_S);
    ui->pushStop->setShortcut(Qt::Key_P);
    ui->pushEllipse->setShortcut(Qt::Key_E);
    ui->pushEllipseFill->setShortcut(Qt::ALT + Qt::Key_E);
    ui->pushDelete->setShortcut(QKeySequence::Delete);
    ui->pushSelectAll->setShortcut(QKeySequence::SelectAll);

    layout()->activate();

    const QRect rc = QDesktopWidget().availableGeometry(window());

    const int pixmapWidth = px.width() + 4;
    const int pixmapHeight = px.height() + 4;

    int neww = width() + pixmapWidth - ui->scrollArea->width();
    int newh = height() + pixmapHeight - ui->scrollArea->height();

    // new window size
    neww = qBound(width(), neww, static_cast<int>(rc.width() * 0.75));
    newh = qBound(height(), newh, static_cast<int>(rc.height() * 0.75));

    // new scrollarea size
    int newSw = ui->scrollArea->width() + (neww - width());
    int newSh = ui->scrollArea->height() + (newh - height());

    // now check for scrollbars
    if(newSh < pixmapHeight && newSw >= pixmapWidth)
        neww += ui->scrollArea->verticalScrollBar()->sizeHint().width();
    else if(newSw < pixmapWidth && newSh >= pixmapHeight)
        newh += ui->scrollArea->horizontalScrollBar()->sizeHint().height();

    resize(neww, newh);
}

ScreenshotEditor::~ScreenshotEditor()
{
    delete ui;
}

QPixmap ScreenshotEditor::renderPixmap()
{
    return ui->scrollAreaWidgetContents->renderPixmap();
}

int ScreenshotEditor::exec()
{
    ui->scrollAreaWidgetContents->restoreLabels();

    // colors
    setEllipseFillColor(SETTINGS_GET_COLOR(SETTING_ELLIPSE_FILL_COLOR));

    m_oellipseFillColor = m_ellipseFillColor;

    int code = QDialog::exec();

    if(code == QDialog::Accepted)
        ui->scrollAreaWidgetContents->saveLabels();
    else
    {
        ui->scrollAreaWidgetContents->clearLabels();

        SETTINGS_SET_COLOR(SETTING_ELLIPSE_FILL_COLOR, m_oellipseFillColor);
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

void ScreenshotEditor::setEllipseFillColor(const QColor &c)
{
    m_ellipseFillColor = c;
    m_ellipseFillColor.setAlpha(70);

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

void ScreenshotEditor::slotEllipseFillColor()
{
    QColorDialog cpw(this);

    QColor c = m_ellipseFillColor;
    c.setAlpha(255);

    cpw.setCurrentColor(c);

    if(cpw.exec() == QDialog::Accepted)
    {
        setEllipseFillColor(cpw.selectedColor());
        SETTINGS_SET_COLOR(SETTING_ELLIPSE_FILL_COLOR, m_ellipseFillColor);
    }
}
