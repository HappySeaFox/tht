#include <QMutableListIterator>
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
    m_pixmaps[0] = QPixmap(":/images/cursor-buy.png");
    m_pixmaps[1] = QPixmap(":/images/cursor-sail.png");
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
    foreach(SelectableLabel *l, m_labels)
    {
        l->setSelected(false);
    }

    return QPixmap::grabWidget(this);
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

void ScreenshotEditorWidget::restoreLabels()
{
    qDebug("Restore labels");

    clearLabels();
    m_labels = m_savedLabels;

    foreach(SelectableLabel *l, m_labels)
    {
        l->show();
    }
}

void ScreenshotEditorWidget::saveLabels()
{
    qDebug("Save labels");

    m_savedLabels = m_labels;
    m_labels.clear();
}

void ScreenshotEditorWidget::clearLabels()
{
    qDebug("Clear labels");

    QList<SelectableLabel *> toDelete;

    foreach(SelectableLabel *l, m_labels)
    {
        if(m_savedLabels.indexOf(l) < 0)
            toDelete.append(l);
    }

    qDeleteAll(toDelete);
}

void ScreenshotEditorWidget::startBuy()
{
    qDebug("Add buy");

    m_editType = Buy;
    setCursor(QCursor(m_pixmaps[m_editType]));
}

void ScreenshotEditorWidget::startSell()
{
    qDebug("Add sell");

    m_editType = Sell;
    setCursor(QCursor(m_pixmaps[m_editType]));
}

void ScreenshotEditorWidget::startStop()
{
    qDebug("Add stop");

    m_editType = Stop;
    setCursor(QCursor(m_pixmaps[m_editType]));
}

void ScreenshotEditorWidget::startText()
{
    qDebug("Add text");

    m_editType = Text;

    ScreenshotCommentInput sci(this);

    if(sci.exec() == QDialog::Accepted)
    {
        sci.saveSettings();

        m_textPixmap = sci.pixmap();

        if(m_textPixmap.isNull())
        {
            cancel();
            return;
        }

        setCursor(m_textPixmap);
    }
    else
        m_editType = None;
}

void ScreenshotEditorWidget::deleteSelected()
{
    QList<SelectableLabel *> toDelete;

    QMutableListIterator<SelectableLabel *> it(m_labels);
    SelectableLabel *l;

    while(it.hasNext())
    {
        l = it.next();

        if(l->selected())
        {
            if(m_savedLabels.indexOf(l) < 0)
                toDelete.append(l);
            else
            {
                it.remove();
                l->hide();
                l->setSelected(false, false);
            }
        }
    }

    qDeleteAll(toDelete);
}

void ScreenshotEditorWidget::selectAll(bool select)
{
    qDebug("Select all labels: %s", select ? "yes" : "no");

    foreach(SelectableLabel *l, m_labels)
    {
        l->setSelected(select, false);
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

void ScreenshotEditorWidget::slotDestroyed()
{
    SelectableLabel *l = reinterpret_cast<SelectableLabel *>(sender());

    m_labels.removeAll(l);
    m_savedLabels.removeAll(l);
}

void ScreenshotEditorWidget::mousePressEvent(QMouseEvent *e)
{
    m_wasPress = true;

    QWidget::mousePressEvent(e);
}

void ScreenshotEditorWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    if(m_editType == None || !m_wasPress)
    {
        // deselect all
        selectAll(false);
        return;
    }

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
    connect(l, SIGNAL(destroyed()), this, SLOT(slotDestroyed()));

    l->move(pt - cursor().hotSpot() - QPoint(2, 2));
    l->show();

    m_labels.append(l);

    return l;
}
