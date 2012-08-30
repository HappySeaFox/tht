#include <QFontMetrics>
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

    // color
    setColor(Settings::instance()->screenshotTextColor());

    // text alignment
    Qt::Alignment align = Settings::instance()->screenshotTextAlignment();

    switch(align)
    {
        case Qt::AlignCenter: ui->pushAlignCenter->setChecked(true); break;
        case Qt::AlignRight: ui->pushAlignRight->setChecked(true); break;

        default:
            ui->pushAlighLeft->setChecked(true);
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

void ScreenshotCommentInput::setColor(const QColor &c)
{
    m_color = c;

    QPixmap px(16, 16);
    px.fill(c);

    ui->pushColor->setIcon(px);
    ui->text->setTextColor(c);

    Settings::instance()->setScreenshotTextColor(c);
}

void ScreenshotCommentInput::slotFontDown()
{
    QFont f = ui->text->font();

    if(f.pointSize() < 3)
        return;

    f.setPointSize(f.pointSize()-1);
    ui->text->setFont(f);

    Settings::instance()->setScreenshotTextSize(f.pointSize());
}

void ScreenshotCommentInput::slotFontUp()
{
    QFont f = ui->text->font();
    f.setPointSize(f.pointSize()+1);
    ui->text->setFont(f);

    Settings::instance()->setScreenshotTextSize(f.pointSize());
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

    Qt::AlignmentFlag align = Qt::AlignLeft;

    if(b == ui->pushAlignCenter)
        align = Qt::AlignCenter;
    else if(b == ui->pushAlignRight)
        align = Qt::AlignRight;

    ui->text->setAlignment(align);
    Settings::instance()->setScreenshotTextAlignment(align);
}
