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

#include <QApplication>
#include <QMouseEvent>
#include <QCursor>
#include <QPixmap>

#include "target.h"

Target::Target(QWidget *parent) :
    QLabel(parent)
{
    m_dragging = false;

    setPixmap(QPixmap(":/images/drag.png"));
    setMouseTracking(true);
}

void Target::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        qDebug("THT: Start dragging");
        QApplication::setOverrideCursor(QCursor(*pixmap()));
        m_dragging = true;
    }
    else if(event->button() == Qt::MiddleButton)
        emit middleClick();
}

void Target::mouseMoveEvent(QMouseEvent *event)
{
    if(m_dragging && event->buttons() == Qt::NoButton)
    {
        QApplication::restoreOverrideCursor();
        m_dragging = false;
    }
}

void Target::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;

    m_dragging = false;

    QPoint p = QCursor::pos();

    qDebug("THT: Dropped at %d,%d", p.x(), p.y());
    QApplication::restoreOverrideCursor();

    emit dropped(p);
}
