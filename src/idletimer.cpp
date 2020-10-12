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
#include <QTimer>
#include <QEvent>

#include "idletimer.h"

IdleTimer::IdleTimer(QObject *parent) :
    QObject(parent),
    m_enabled(true)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(5000);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SIGNAL(idle()));

    qApp->installEventFilter(this);
}

void IdleTimer::setInterval(int msec)
{
    m_timer->setInterval(msec);
}

void IdleTimer::setEnabled(bool enab)
{
    m_enabled = enab;

    if(m_enabled)
        m_timer->start();
    else
        m_timer->stop();
}

bool IdleTimer::eventFilter(QObject *o, QEvent *e)
{
    switch(e->type())
    {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove:
        case QEvent::Wheel:
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        if(m_enabled)
            m_timer->start();
        break;

        default:
        break;
    }

    return QObject::eventFilter(o, e);
}
