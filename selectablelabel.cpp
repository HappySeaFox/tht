#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

#include "selectablelabel.h"

SelectableLabel::SelectableLabel(const QPixmap &px, const QPoint &startPoint, const QPoint &endPoint, const QColor &cl, QWidget *parent)
    : QLabel(parent),
      m_selected(true),
      m_vectorColor(cl)
{
    m_pixmap = new QLabel(this);

    m_pixmap->setAlignment(Qt::AlignCenter);
    m_pixmap->setPixmap(px);
    m_pixmap->installEventFilter(this);
    m_pixmap->setFixedSize(px.size() + QSize(4,4));

    // move & resize
    setFixedSize(qAbs(endPoint.x() - startPoint.x()) + m_pixmap->width(),
                 qAbs(endPoint.y() - startPoint.y()) + m_pixmap->height());

    QPoint coordinates;

    // pixmap coordinates
    if(endPoint.x() >= startPoint.x())
    {
        coordinates.setX(endPoint.x() - startPoint.x());
        m_vectorStart.setX(coordinates.x());
        m_vectorEnd.setX(0);
    }
    else
    {
        coordinates.setX(0);
        m_vectorStart.setX(m_pixmap->width());
        m_vectorEnd.setX(width());
    }

    if(endPoint.y() >= startPoint.y())
    {
        coordinates.setY(endPoint.y() - startPoint.y());
        m_vectorStart.setY(coordinates.y());
        m_vectorEnd.setY(0);
    }
    else
    {
        coordinates.setY(0);
        m_vectorStart.setY(m_pixmap->height());
        m_vectorEnd.setY(height());
    }

    m_pixmap->move(coordinates);

    // this label coordinates
    if(endPoint.x() >= startPoint.x())
        coordinates.setX(startPoint.x());
    else
        coordinates.setX(endPoint.x()-m_pixmap->width());

    if(endPoint.y() >= startPoint.y())
        coordinates.setY(startPoint.y());
    else
        coordinates.setY(endPoint.y()-m_pixmap->height());

    if(m_vectorStart == m_vectorEnd)
        coordinates -= QPoint(2, 2);

    move(coordinates);

    setSelected(false, false);
}

void SelectableLabel::setSelected(bool s, bool loud)
{
    if(s == m_selected)
        return;

    m_selected = s;

    m_pixmap->setStyleSheet(QString("QLabel{ border: 2px solid %1; }").arg(m_selected ? "red" : "transparent"));

    if(loud)
        emit selected(m_selected);
}

void SelectableLabel::mousePressEvent(QMouseEvent *e)
{
    e->ignore();
}

void SelectableLabel::mouseReleaseEvent(QMouseEvent *e)
{
    e->ignore();
}

bool SelectableLabel::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)

    QEvent::Type type = event->type();

    switch(type)
    {
        case QEvent::MouseButtonPress:
        {
            m_wasPress = true;
        }
        return true;

        case QEvent::MouseButtonRelease:
        {
            if(m_wasPress)
                setSelected(true);

            m_wasPress = false;
        }
        return true;

        default:
            break;
    }

    return false;
}

void SelectableLabel::paintEvent(QPaintEvent *pe)
{
    QLabel::paintEvent(pe);

    if(m_vectorStart == m_vectorEnd)
        return;

    QPainter p(this);

    p.setClipRect(pe->rect());

    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    p.setPen(m_vectorColor);

    p.drawLine(m_vectorStart, m_vectorEnd);
}
