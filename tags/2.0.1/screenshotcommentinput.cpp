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
#include <QColorDialog>
#include <QPushButton>
#include <QPainter>
#include <QFont>

#include "screenshotcommentinput.h"
#include "thtsettings.h"
#include "settings.h"
#include "ui_screenshotcommentinput.h"

ScreenshotCommentInput::ScreenshotCommentInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreenshotCommentInput)
{
    ui->setupUi(this);

    m_originalBackgroundColor = ui->text->palette().color(QPalette::Base);

    QWidgetList buttons = QWidgetList()
                             << ui->pushAlignLeft
                             << ui->pushAlignCenter
                             << ui->pushAlignRight
                             << ui->pushFontDown
                             << ui->pushFontUp
                             << ui->pushColor
                             << ui->pushBackgroundColor;

    foreach(QWidget *w, buttons)
    {
        w->setFixedSize(22, 22);
    }

    // text itself
    ui->text->setFocus();
    ui->text->setText(SETTINGS_GET_STRING(SETTING_SCREENSHOT_TEXT));
    ui->text->selectAll();

    // colors
    ui->checkBackgroundColor->setChecked(SETTINGS_GET_BOOL(SETTING_SCREENSHOT_USE_BACKGROUND_COLOR));
    setColor(SETTINGS_GET_COLOR(SETTING_SCREENSHOT_TEXT_COLOR));
    setBackgroundColor(SETTINGS_GET_COLOR(SETTING_SCREENSHOT_BACKGROUND_COLOR));

    // text alignment
    m_align = SETTINGS_GET_ALIGNMENT_FLAG(SETTING_SCREENSHOT_TEXT_ALIGNMENT);

    switch(m_align)
    {
        case Qt::AlignCenter: ui->pushAlignCenter->setChecked(true); break;
        case Qt::AlignRight: ui->pushAlignRight->setChecked(true); break;

        default:
            ui->pushAlignLeft->setChecked(true);
        break;
    }

    // text size
    int size = SETTINGS_GET_INT(SETTING_SCREENSHOT_TEXT_SIZE);

    if(size > 0)
    {
        QFont f = ui->text->font();
        f.setPointSize(size);
        ui->text->setFont(f);
    }

    // shortcuts
    ui->pushAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    ui->pushAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    ui->pushAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);

    ui->pushFontUp->setShortcut(QKeySequence::ZoomIn);
    ui->pushFontDown->setShortcut(QKeySequence::ZoomOut);

    ui->pushColor->setShortcut(Qt::CTRL + Qt::Key_K);
    ui->pushBackgroundColor->setShortcut(Qt::CTRL + Qt::Key_B);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setShortcut(Qt::CTRL + Qt::Key_Return);
}

ScreenshotCommentInput::~ScreenshotCommentInput()
{
    delete ui;
}

QPixmap ScreenshotCommentInput::pixmap() const
{
    QString text = ui->text->toPlainText();

    if(text.isEmpty())
        return QPixmap();

    QFontMetrics fm(ui->text->font());
    QSize size = fm.boundingRect(QRect(), ui->text->alignment(), text).size();
    bool withBackground = false;

    const int enlargeWidth = 3;
    const int enlargeHeight = 2;

    if(ui->checkBackgroundColor->isChecked())
    {
        withBackground = true;
        size += QSize(enlargeWidth*2, enlargeHeight*2);
    }

    QPixmap px(size);
    px.fill(Qt::transparent);
    QPainter p(&px);

    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // draw text
    p.setFont(ui->text->font());

    if(withBackground)
        p.fillRect(px.rect(), m_backgroundColor);

    p.setPen(m_color);
    p.drawText(withBackground
               ? px.rect().adjusted(enlargeWidth, enlargeHeight, -enlargeWidth, -enlargeHeight)
               : px.rect(),
               ui->text->alignment(), text);
    p.end();

    return px;
}

void ScreenshotCommentInput::saveSettings() const
{
    SETTINGS_SET_ALIGNMENT_FLAG(SETTING_SCREENSHOT_TEXT_ALIGNMENT, m_align, Settings::NoSync);
    SETTINGS_SET_INT(SETTING_SCREENSHOT_TEXT_SIZE, ui->text->font().pointSize(), Settings::NoSync);
    SETTINGS_SET_COLOR(SETTING_SCREENSHOT_TEXT_COLOR, m_color, Settings::NoSync);
    SETTINGS_SET_BOOL(SETTING_SCREENSHOT_USE_BACKGROUND_COLOR, ui->checkBackgroundColor->isChecked(), Settings::NoSync);
    SETTINGS_SET_COLOR(SETTING_SCREENSHOT_BACKGROUND_COLOR, m_backgroundColor, Settings::NoSync);
    SETTINGS_SET_STRING(SETTING_SCREENSHOT_TEXT, ui->text->toPlainText()); // also sync
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

void ScreenshotCommentInput::setBackgroundColor(const QColor &c)
{
    m_backgroundColor = c;

    QPixmap px(16, 16);
    px.fill(c);

    ui->pushBackgroundColor->setIcon(px);

    if(ui->checkBackgroundColor->isChecked())
        slotUseBackgroundColor(true);
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
    QColorDialog cpw(this);

    cpw.setCurrentColor(m_color);

    if(cpw.exec() == QDialog::Accepted)
    {
        setColor(cpw.selectedColor());
    }
}

void ScreenshotCommentInput::slotChangeBackgroundColor()
{
    QColorDialog cpw(this);

    cpw.setCurrentColor(m_backgroundColor);

    if(cpw.exec() == QDialog::Accepted)
    {
        setBackgroundColor(cpw.selectedColor());
    }
}

void ScreenshotCommentInput::slotUseBackgroundColor(bool use)
{
    QPalette pal = ui->text->palette();
    pal.setColor(QPalette::Base, use ? m_backgroundColor : m_originalBackgroundColor);
    ui->text->setPalette(pal);
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
