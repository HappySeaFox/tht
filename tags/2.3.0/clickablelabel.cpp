/*
 * This file is part of THT-lib.
 *
 * THT-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation.
 *
 * THT-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QMouseEvent>

#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent) :
    QLabel(parent),
    m_clicked(false)
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

ClickableLabel::~ClickableLabel()
{}

bool ClickableLabel::event(QEvent *e)
{
    QEvent::Type type = e->type();

    switch(type)
    {
        case QEvent::MouseButtonPress:
            m_clicked = true;
        return true;

        case QEvent::MouseButtonRelease:
            if(m_clicked)
                emit clicked();

            m_clicked = false;
        return true;

        default:
        break;
    }

    return QLabel::event(e);
}
