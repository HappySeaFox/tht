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

#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QPixmap>

#include "selectablelabel.h"

static const int SL_MARGIN = 2;

SelectableLabel::SelectableLabel(const QPixmap &px, const QPoint &startPoint, const QPoint &endPoint, const QColor &cl, QWidget *parent) :
    QLabel(parent),
    m_selected(true),
    m_vectorStart(startPoint),
    m_vectorEnd(endPoint),
    m_vectorColor(cl)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setPixmap(px);
    setLineWidth(SL_MARGIN);
    setFrameStyle(QFrame::Box);
    setFixedSize(px.size() + QSize(SL_MARGIN*2, SL_MARGIN*2));

    QPoint coordinates;

    // our coordinates
    coordinates.setX((endPoint.x() >= startPoint.x()) ? endPoint.x() : endPoint.x()-width());
    coordinates.setY((endPoint.y() >= startPoint.y()) ? endPoint.y() : endPoint.y()-height());

    // just pixmap or pixmap+vector?
    if(startPoint == endPoint)
        coordinates -= QPoint(SL_MARGIN, SL_MARGIN);
    else
        coordinates += QPoint((endPoint.x() >= startPoint.x()) ? -1 : 1,
                              (endPoint.y() >= startPoint.y()) ? -1 : 1);

    move(coordinates);

    setSelected(false, false);
}

void SelectableLabel::setSelected(bool s, bool loud)
{
    if(s == m_selected)
        return;

    m_selected = s;

    update();

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

void SelectableLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setClipRect(event->rect());

    if(m_selected)
    {
        painter.setPen(QPen(m_vectorColor, SL_MARGIN*2));
        painter.drawRect(rect());
    }

    painter.drawPixmap(SL_MARGIN, SL_MARGIN, *pixmap());
}
