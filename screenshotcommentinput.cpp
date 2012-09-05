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
#include <QFontMetrics>
#include <QShortcut>
#include <QPainter>
#include <QFont>

#include "screenshotcommentinput.h"
#include "colorpickerwidget.h"
#include "settings.h"
#include "ui_screenshotcommentinput.h"

ScreenshotCommentInput::ScreenshotCommentInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreenshotCommentInput)
{
    ui->setupUi(this);

    ui->text->setFocus();

    QWidgetList buttons = QWidgetList()
                             << ui->pushAlignLeft
                             << ui->pushAlignCenter
                             << ui->pushAlignRight
                             << ui->pushFontDown
                             << ui->pushFontUp
                             << ui->pushColor;

    foreach(QWidget *w, buttons)
    {
        w->setFixedSize(22, 22);
    }

    // text itself
    ui->text->setText(Settings::instance()->screenshotText());
    ui->text->selectAll();

    // color
    setColor(Settings::instance()->screenshotTextColor());

    // text alignment
    m_align = Settings::instance()->screenshotTextAlignment();

    switch(m_align)
    {
        case Qt::AlignCenter: ui->pushAlignCenter->setChecked(true); break;
        case Qt::AlignRight: ui->pushAlignRight->setChecked(true); break;

        default:
            ui->pushAlignLeft->setChecked(true);
        break;
    }

    // text size
    int size = Settings::instance()->screenshotTextSize();

    if(size > 0)
    {
        QFont f = ui->text->font();
        f.setPointSize(size);
        ui->text->setFont(f);
    }

    // shortcuts
    new QShortcut(QKeySequence::ZoomIn, ui->pushFontUp, SLOT(animateClick()));
    new QShortcut(QKeySequence::ZoomOut, ui->pushFontDown, SLOT(animateClick()));
    new QShortcut(Qt::CTRL+Qt::Key_Return, ui->buttonBox->button(QDialogButtonBox::Ok), SLOT(animateClick()));
    new QShortcut(Qt::CTRL+Qt::Key_L, ui->pushAlignLeft, SLOT(animateClick()));
    new QShortcut(Qt::CTRL+Qt::Key_E, ui->pushAlignCenter, SLOT(animateClick()));
    new QShortcut(Qt::CTRL+Qt::Key_R, ui->pushAlignRight, SLOT(animateClick()));
}

ScreenshotCommentInput::~ScreenshotCommentInput()
{
    delete ui;
}

QPixmap ScreenshotCommentInput::pixmap()
{
    QString text = ui->text->toPlainText();

    if(text.isEmpty())
        return QPixmap();

    QFontMetrics fm(ui->text->font());
    QSize size = fm.boundingRect(QRect(), ui->text->alignment(), text).size();

    QPixmap px(size);
    px.fill(Qt::transparent);
    QPainter p(&px);

    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // draw text
    p.setFont(ui->text->font());
    p.setPen(m_color);
    p.drawText(px.rect(), ui->text->alignment(), text);
    p.end();

    return px;
}

void ScreenshotCommentInput::saveSettings()
{
    Settings::instance()->setScreenshotTextColor(m_color, Settings::NoSync);
    Settings::instance()->setScreenshotTextSize(ui->text->font().pointSize(), Settings::NoSync);
    Settings::instance()->setScreenshotTextAlignment(m_align, Settings::NoSync);
    Settings::instance()->setScreenshotText(ui->text->toPlainText()); // also sync
}

void ScreenshotCommentInput::setColor(const QColor &c)
{
    m_color = c;

    QPixmap px(16, 16);
    px.fill(c);

    ui->pushColor->setIcon(px);

    QPalette pal = ui->text->palette();

    pal.setColor(QPalette::WindowText, c);
    pal.setColor(QPalette::Text, c);

    ui->text->setPalette(pal);
}

void ScreenshotCommentInput::slotFontDown()
{
    QFont f = ui->text->font();

    if(f.pointSize() < 3)
        return;

    f.setPointSize(f.pointSize()-1);
    ui->text->setFont(f);
}

void ScreenshotCommentInput::slotFontUp()
{
    QFont f = ui->text->font();
    f.setPointSize(f.pointSize()+1);
    ui->text->setFont(f);
}

void ScreenshotCommentInput::slotChangeColor()
{
    ColorPickerWidget cpw(this);

    cpw.setColor(m_color);

    if(cpw.exec() == QDialog::Accepted)
    {
        setColor(cpw.color());
    }
}

void ScreenshotCommentInput::slotAlignChanged(bool checked)
{
    if(!checked)
        return;

    QToolButton *b = qobject_cast<QToolButton *>(sender());

    if(!b)
        return;

    m_align = Qt::AlignLeft;

    if(b == ui->pushAlignCenter)
        m_align = Qt::AlignCenter;
    else if(b == ui->pushAlignRight)
        m_align = Qt::AlignRight;

    ui->text->setUpdatesEnabled(false);

    // navigate through the all paragraphs and set alignment
    QTextCursor oldCursor = ui->text->textCursor();

    ui->text->moveCursor(QTextCursor::Start);

    int lastPos = -1;
    int curPos = 0;

    while(lastPos != curPos)
    {
       ui->text->setAlignment(m_align);
       ui->text->moveCursor(QTextCursor::Down);
       lastPos = curPos;
       curPos = ui->text->textCursor().position();
    }

    ui->text->setTextCursor(oldCursor);
    ui->text->setUpdatesEnabled(true);
}
