#include <QMouseEvent>
#include <QCursor>
#include <QPixmap>
#include <QTimer>

#include "screenshoteditorwidget.h"
#include "screenshotcommentinput.h"
#include "selectablelabel.h"

ScreenshotEditorWidget::ScreenshotEditorWidget(QWidget *parent) :
    QLabel(parent),
    m_editType(None),
    m_wasPress(false)
{
    m_pixmaps[0] = QPixmap(":/images/cursor-long.png");
    m_pixmaps[1] = QPixmap(":/images/cursor-short.png");
    m_pixmaps[2] = QPixmap(":/images/cursor-stop.png");
}

void ScreenshotEditorWidget::setPixmap(const QPixmap &p)
{
    if(!p.isNull())
        setFixedSize(p.size());

    QLabel::setPixmap(p);
}

QPixmap ScreenshotEditorWidget::pixmap()
{
    QList<SelectableLabel *> list = labels();

    foreach(SelectableLabel *l, list)
    {
        l->setSelected(false);
    }

    return QPixmap::grabWidget(this);
}

QList<SelectableLabel *> ScreenshotEditorWidget::labels()
{
    return findChildren<SelectableLabel *>();
}

void ScreenshotEditorWidget::cancel()
{
    if(m_editType != None)
    {
        m_editType = None;
        m_wasPress = false;
        slotResetCursor();
    }
}

void ScreenshotEditorWidget::startLong()
{
    m_editType = Long;
    setCursor(QCursor(m_pixmaps[m_editType]));
}

void ScreenshotEditorWidget::startShort()
{
    m_editType = Short;
    setCursor(QCursor(m_pixmaps[m_editType]));
}

void ScreenshotEditorWidget::startStop()
{
    m_editType = Stop;
    setCursor(QCursor(m_pixmaps[m_editType]));
}

void ScreenshotEditorWidget::startText()
{
    m_editType = Text;
    ScreenshotCommentInput sci(this);

    if(sci.exec() == QDialog::Accepted)
    {
        m_textPixmap = sci.pixmap();

        if(m_textPixmap.isNull())
        {
            cancel();
            return;
        }

        setCursor(m_textPixmap);
    }
}

void ScreenshotEditorWidget::deleteSelected()
{
    QList<SelectableLabel *> list = labels();

    foreach(SelectableLabel *l, list)
    {
        if(l->selected())
            delete l;
    }
}

void ScreenshotEditorWidget::slotResetCursor()
{
    setCursor(Qt::ArrowCursor);
}

void ScreenshotEditorWidget::slotSelected(bool s)
{
    emit selected(qobject_cast<SelectableLabel *>(sender()), s);
}

void ScreenshotEditorWidget::mousePressEvent(QMouseEvent *e)
{
    e->accept();
    m_wasPress = true;
}

void ScreenshotEditorWidget::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();

    if(m_editType == None || !m_wasPress)
        return;

    if(m_editType != Text)
        addLabel(e->pos(), m_pixmaps[m_editType]);
    else
    {
        addLabel(e->pos(), m_textPixmap);
        m_textPixmap = QPixmap();
    }

    m_editType = None;
    m_wasPress = false;

    QTimer::singleShot(50, this, SLOT(slotResetCursor()));
}

SelectableLabel *ScreenshotEditorWidget::addLabel(const QPoint &pt, const QPixmap &px)
{
    SelectableLabel *l = new SelectableLabel(px, this);

    connect(l, SIGNAL(selected(bool)), this, SLOT(slotSelected(bool)));

    l->move(pt - cursor().hotSpot() - QPoint(2, 2));
    l->show();

    return l;
}
