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
#include <QHBoxLayout>
#include <QCursor>
#include <QPixmap>
#include <QPoint>
#include <QLabel>

#include "numericlabel.h"
#include "target.h"
#include "tools.h"

Target::Target(QWidget *parent) :
    QWidget(parent)
{
    m_dragging = false;

    m_drag_black = QPixmap(":/images/drag.png");
    m_drag_red = QPixmap(":/images/drag_red.png");

    QHBoxLayout *l = new QHBoxLayout;
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);

    m_label = new QLabel(this);
    m_label->setFixedSize(32, 32);
    m_label->setPixmap(m_drag_black);
    m_label->setToolTip(tr("Drag and drop this target to the window you need to create a link to"));

    m_number = new NumericLabel(this);

    l->addWidget(m_label);
    l->addStretch(1);

    setFixedWidth(m_label->width() + 14);

    // move the number to the bottom
    m_number->move(width() - m_number->width(), height() - m_number->height());

    setMouseTracking(true);

    setWhatsThis(QString("<a href=\"http://www.youtube.com/playlist?list=PL5FURm9nDau8oTXumieXJl3DNDRTUlBSm\">%1</a>")
                 .arg(Tools::openYoutubeTutorialTitle()));

    qApp->installEventFilter(this);
}

bool Target::mayBeMaster() const
{
    return (QApplication::keyboardModifiers() & Qt::AltModifier);
}

void Target::setNumberOfLinks(uint n)
{
    m_number->setValue(n);

    if(!n)
        setNumberToolTip(QString());
}

void Target::setNumberToolTip(const QString &tip)
{
    m_number->setToolTip(tip);
}

void Target::locked(bool l)
{
    m_number->setEnabled(!l);
}

void Target::mousePressEvent(QMouseEvent *event)
{
    if(!m_label->rect().contains(event->pos()))
        return;

    if(event->button() == Qt::LeftButton)
    {
        qDebug("Start dragging");
        QApplication::setOverrideCursor(QCursor(*m_label->pixmap()));
        m_dragging = true;
    }
    else if(event->button() == Qt::MiddleButton)
        emit middleClick();
}

void Target::mouseMoveEvent(QMouseEvent *event)
{
    if(m_dragging)
    {
        if(event->buttons() == Qt::NoButton)
        {
            QApplication::restoreOverrideCursor();
            m_dragging = false;
            emit cancelled();
        }
        else
            emit moving(event->globalPos());
    }
}

void Target::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton || !m_dragging)
        return;

    m_dragging = false;

    QPoint p = QCursor::pos();

    qDebug("Dropped at %d,%d", p.x(), p.y());
    QApplication::restoreOverrideCursor();

    emit dropped(p);
}

void Target::enterEvent(QEvent *e)
{
    Q_UNUSED(e)

    if(mayBeMaster())
        m_label->setPixmap(m_drag_red);
}

void Target::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)

    m_label->setPixmap(m_drag_black);
}

void Target::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)

    m_number->move(width() - m_number->width(), height() - m_number->height());
}

bool Target::eventFilter(QObject *o, QEvent *e)
{
    QEvent::Type type = e->type();

    if(type == QEvent::KeyPress || type == QEvent::KeyRelease)
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        QPoint cursorPos = mapFromGlobal(QCursor::pos());

        if(ke && rect().contains(cursorPos) && ke->key() == Qt::Key_Alt)
        {
            m_label->setPixmap((type == QEvent::KeyPress) ? m_drag_red : m_drag_black);
            return true;
        }
    }

    return QObject::eventFilter(o, e);
}
