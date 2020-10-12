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

#ifndef COLORANIMATION_H
#define COLORANIMATION_H

#include <QObject>

class QPropertyAnimation;
class QWidget;

class ColorAnimation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int colorAlpha READ colorAlpha WRITE setColorAlpha)

public:
    explicit ColorAnimation(QWidget *target, QObject *parent = 0);

    void start();
    void stop();

private:
    int colorAlpha() const;
    void setColorAlpha(int);

private slots:
    void slotAnimationFinished();

private:
    QPropertyAnimation *m_animation;
    int m_colorAlpha;
    QWidget *m_target;
    QString m_baseColorString;
};

inline
int ColorAnimation::colorAlpha() const
{
    return m_colorAlpha;
}

#endif // COLORANIMATION_H
