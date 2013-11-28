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

#include <QImage>

#include "numericlabel.h"

NumericLabel::NumericLabel(QWidget *parent)
    : QLabel(parent)
{
    init(0);
}

NumericLabel::NumericLabel(int value, QWidget *parent)
    : QLabel(parent)
{
    init(value);
}

NumericLabel::~NumericLabel()
{}

void NumericLabel::setValue(int val)
{
    if(val < 0 || m_value == val || (m_value > 20 && val > 20))
        return;

    m_value = val;

    QHash<uint, QPixmap>::iterator it = m_cache.find(m_value > 20 ? 21 : m_value);
    QPixmap pixmap;

    if(it == m_cache.end())
    {
        pixmap = QPixmap(m_value > 20 ? ":/images/counters/20+.png" : QString(":/images/counters/%1.png").arg(m_value));
        m_cache.insert(m_value > 20 ? 21 : m_value, pixmap);
    }
    else
        pixmap = it.value();

    setPixmap(pixmap);
}

void NumericLabel::init(int value)
{
    m_value = -1;

    setContentsMargins(0, 0, 0, 0);
    setValue(value);

    QImage im(":/images/counters/20+.png");

    if(!im.isNull())
        setFixedSize(im.size());
    else
    {
        qWarning("The reference \"20+\" image is broken");
        setFixedSize(15, 9);
    }
}
