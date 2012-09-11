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

#include <QLinearGradient>
#include <QVBoxLayout>
#include <QPalette>
#include <QPainter>
#include <QPixmap>
#include <QFrame>
#include <QRect>

#include "listdetails.h"

ListDetails::ListDetails(QWidget *parent) :
    QLabel(parent)
{
    setObjectName("ListDetails");

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setSpacing(2);
    l->setContentsMargins(2, 2, 3, 3);
    setLayout(l);

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setMinimumWidth(18);
    setFrameShape(QFrame::NoFrame);

    // current ticker
    m_current = new QLabel(this);
    m_current->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_current->setTextFormat(Qt::PlainText);
    l->addWidget(m_current);

    // separator
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::Box);
    line->setFixedHeight(1);
    QPalette pal = line->palette();
    pal.setColor(QPalette::Foreground, Qt::gray);
    line->setPalette(pal);
    l->addWidget(line);

    // total tickers
    m_total = new QLabel(this);
    m_total->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_total->setTextFormat(Qt::PlainText);
    l->addWidget(m_total);
}

void ListDetails::resetBackground()
{
    QPixmap px(size());
    QPainter p(&px);

    // gradient
    QLinearGradient gradient(0, 0, 0, height());

    gradient.setColorAt(0.0,  QColor(0xFF, 0xEF, 0xEF));
    gradient.setColorAt(0.35, QColor(0xF7, 0xDB, 0x45));
    gradient.setColorAt(0.65, QColor(0xF7, 0xDB, 0x45));
    gradient.setColorAt(1.0,  QColor(0xFF, 0xEF, 0xEF));

    p.fillRect(rect(), gradient);

    // border
    p.setPen(QColor(128, 128, 128));

    QRect rc = rect();
    rc.setBottomRight(rc.bottomRight() - QPoint(1,1));
    p.drawRect(rc);

    setPixmap(px);
}

