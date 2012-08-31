#include <QMouseEvent>
#include <QPixmap>

#include "selectablelabel.h"

static const int SL_MARGIN = 2;

SelectableLabel::SelectableLabel(const QPixmap &px, const QPoint &startPoint, const QPoint &endPoint, const QColor &cl, QWidget *parent)
    : QLabel(parent),
      m_selected(true),
      m_vectorStart(startPoint),
      m_vectorEnd(endPoint),
      m_vectorColor(cl)
{
    setAlignment(Qt::AlignCenter);
    setPixmap(px);
    setLineWidth(SL_MARGIN);
    setFrameStyle(QFrame::Box);
    setFixedSize(px.size() + QSize(SL_MARGIN*2, SL_MARGIN*2));

    QPoint coordinates;

    // our coordinates
    coordinates.setX((endPoint.x() >= startPoint.x()) ? endPoint.x() : endPoint.x()-width());
    coordinates.setY((endPoint.y() >= startPoint.y()) ? endPoint.y() : endPoint.y()-height());

    if(startPoint == endPoint)
        coordinates -= QPoint(SL_MARGIN, SL_MARGIN);

    move(coordinates);

    setSelected(false, false);
}

void SelectableLabel::setSelected(bool s, bool loud)
{
    if(s == m_selected)
        return;

    m_selected = s;

    QPalette pal = palette();
    pal.setColor(QPalette::Foreground, m_selected ? Qt::red : Qt::transparent);
    setPalette(pal);

    if(loud)
        emit selected(m_selected);
}

void SelectableLabel::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    m_wasPress = true;
}

void SelectableLabel::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    if(m_wasPress)
        setSelected(true);

    m_wasPress = false;
}
