/*

Copyright (C) 2009, Etienne Moutot <e.moutot@gmail.com>

This file is part of colorPickerWidget.

    colorPickerWidget is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    colorPickerWidget is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "colorviewer.h"

ColorViewer::ColorViewer(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(10, 10);
}

ColorViewer::~ColorViewer()
{
}

void ColorViewer::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setPen(actualPen);
    p.setBrush(QBrush(actualColor));
    p.drawRect( QRect( 2, 2, width()-4, height()-4 ) );
}

void ColorViewer::setPen(const QPen &pen)
{
    actualPen = pen;
}

QPen ColorViewer::pen() const
{
    return actualPen;
}

void ColorViewer::setColor(const QColor &color)
{
    actualColor = color;
}
QColor ColorViewer::color() const
{
    return actualColor;
}


void ColorViewer::changeColor(const QColor &color)
{
    actualColor = color;
    update();
}
