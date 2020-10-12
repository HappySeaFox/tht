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

#include <QPropertyAnimation>
#include <QWidget>
#include <QPalette>
#include <QColor>

#include "coloranimation.h"

ColorAnimation::ColorAnimation(QWidget *target, QObject *parent) :
    QObject(parent),
    m_colorAlpha(255),
    m_target(target)
{
    m_animation = new QPropertyAnimation(this, "colorAlpha", this);
    m_animation->setDuration(750);
    m_animation->setStartValue(255);
    m_animation->setEndValue(32);
    m_animation->setLoopCount(1);

    QColor color = m_target->palette().color(QPalette::WindowText);
    m_baseColorString = QString("%1, %2, %3").arg(color.red()).arg(color.green()).arg(color.blue());

    connect(m_animation, SIGNAL(finished()), this, SLOT(slotAnimationFinished()));
}

void ColorAnimation::start()
{
    m_animation->start();
}

void ColorAnimation::stop()
{
    m_animation->stop();
    m_target->setStyleSheet(QString("color: rgba(%1, 255);").arg(m_baseColorString));
}

void ColorAnimation::slotAnimationFinished()
{
    m_animation->setDirection(m_animation->direction() == QAbstractAnimation::Forward
                              ? QAbstractAnimation::Backward
                              : QAbstractAnimation::Forward);
    m_animation->start();
}

void ColorAnimation::setColorAlpha(int alpha)
{
    m_colorAlpha = alpha;
    m_target->setStyleSheet(QString("color: rgba(%1, %2);").arg(m_baseColorString).arg(m_colorAlpha));
}
