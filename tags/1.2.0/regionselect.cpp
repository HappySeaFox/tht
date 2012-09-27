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

#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>
#include <QCursor>
#include <QPainter>
#include <QEvent>

#include "regionselect.h"

RegionSelect::RegionSelect(KeyboardInteraction _ki, QWidget *parent) :
    QDialog(parent),
    ki(_ki)
{    
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowState(Qt::WindowFullScreen);
    setCursor(Qt::CrossCursor);

    QRect rc = QApplication::desktop()->screenGeometry(QCursor::pos());

    sizeDesktop = rc.size();
    resize(sizeDesktop);

    desktopPixmapBkg = QPixmap::grabWindow(QApplication::desktop()->winId(),
                                           rc.x(), rc.y(), rc.width(), rc.height());

    desktopPixmapClr = desktopPixmapBkg;

    move(rc.topLeft());
    drawBackGround();
}

RegionSelect::~RegionSelect()
{}

bool RegionSelect::event(QEvent *event)
{
    if(ki == UseKeyboard && event->type() == QEvent::KeyPress)
        reject();
    else if(event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);

        if(mouseEvent->button() == Qt::LeftButton)
            accept();
    }
    else if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);

        if(mouseEvent->button() != Qt::LeftButton)
            reject();

        selStartPoint = mouseEvent->pos();
        selectRect = QRect(selStartPoint, QSize());
    }

    return QDialog::event(event);
}

void RegionSelect::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    if(!palBackground)
      painter.drawPixmap(QPoint(0, 0), desktopPixmapBkg);

    drawRectSelection(painter);
}

void RegionSelect::mouseMoveEvent(QMouseEvent *event)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
    selectRect = QRect(selStartPoint, mouseEvent->pos()).normalized();
    selEndPoint = mouseEvent->pos();
    update();
}

void RegionSelect::drawBackGround()
{
    // create painter on  pixelmap of desktop
    QPainter painter(&desktopPixmapBkg);

    // set painter brush on 85% transparency
    painter.setBrush(QBrush(QColor(0, 0, 0, 85), Qt::SolidPattern));

    // draw rect of desktop size in poainter
    painter.drawRect(QApplication::desktop()->rect());
        
    QRect txtRect = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
    QString txtTip = ki == UseKeyboard
            ? tr("Select the rectangle with the mouse or exit pressing\nany key or using the right or middle mouse buttons.")
            : tr("Select the rectangle with the mouse or exit pressing\nthe right or middle mouse buttons.");

    txtRect.setHeight(qRound(txtRect.height() / 10)); // rounded val of text rect height

    painter.setPen(QPen(Qt::red)); // set message rect border color
    painter.setBrush(QBrush(QColor(255, 255, 255, 180), Qt::SolidPattern));
    QRect txtBgRect = painter.boundingRect(txtRect, Qt::AlignCenter, txtTip);

    // set height & width of bkg rect
    txtBgRect.setX(txtBgRect.x() - 6);
    txtBgRect.setY(txtBgRect.y() - 4);
    txtBgRect.setWidth(txtBgRect.width() + 12);
    txtBgRect.setHeight(txtBgRect.height() + 8);

    painter.drawRect(txtBgRect);

    // Draw the text
    painter.setPen(QPen(Qt::black)); // black color pen
    painter.drawText(txtBgRect, Qt::AlignCenter, txtTip);

    palBackground = (qApp->desktop()->numScreens() > 1);

    // set bkg to pallette widget
    if (palBackground)
    {
        QPalette newPalette = palette();
        newPalette.setBrush(QPalette::Window, QBrush(desktopPixmapBkg));
        setPalette(newPalette);
    }
}

void RegionSelect::drawRectSelection(QPainter &painter)
{
    painter.drawPixmap(selectRect, desktopPixmapClr, selectRect);
    painter.setPen(QPen(Qt::white, 1));

    QPainter::CompositionMode cm = painter.compositionMode();
    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    painter.drawRect(selectRect);

    painter.setCompositionMode(cm);
}

QPixmap RegionSelect::selection() const
{
    return selectRect.isValid() ? desktopPixmapClr.copy(selectRect) : QPixmap();
}   
