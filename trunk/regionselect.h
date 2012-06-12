/***************************************************************************
 *   Copyright (C) 2009 by Artem 'DOOMer' Galichkin                        *
 *   doomer3d@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010,2012 by Dmitry 'Krasu' Baryshev                    *
 *   ksquirrel.iv@gmail.com                                                *
 ***************************************************************************/

#ifndef REGIONSELECT_H
#define REGIONSELECT_H

#include <QDialog>
#include <QPixmap>
#include <QPoint>
#include <QRect>

class QEvent;
class QMouseEvent;
class QPainter;
class QLabel;

class RegionSelect : public QDialog        
{
    Q_OBJECT

    public:
        enum KeyboardInteraction {UseKeyboard, DontUseKeyboard };

        explicit RegionSelect(KeyboardInteraction, QWidget *parent = 0);
        ~RegionSelect();

        QPixmap selection();

    protected:
        bool event(QEvent *event);
        void paintEvent(QPaintEvent *event);
        void mouseMoveEvent(QMouseEvent *event);

    private:
        void drawBackGround();
        void drawRectSelection(QPainter &painter);

    private:
        KeyboardInteraction ki;

        QRect selectRect;
        QSize sizeDesktop;

        QPoint selStartPoint;
        QPoint selEndPoint;

        bool palBackground;

        QPixmap desktopPixmapBkg;
        QPixmap desktopPixmapClr;
};

#endif // REGIONSELECT_H
