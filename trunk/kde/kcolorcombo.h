/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)
    Copyright (c) 2007 David Jarvie (software@astrojar.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

/**********************************************************************
 *   Copyright (C) 2014 by Dmitry 'Krasu' Baryshev                    *
 *   linuxsquirrel.dev@gmail.com                                      *
 **********************************************************************/

//-----------------------------------------------------------------------------
// KDE color selection combo box

// layout management added Oct 1997 by Mario Weilguni
// <mweilguni@sime.com>


#ifndef KCOLORCOMBO_H
#define KCOLORCOMBO_H

#include <QComboBox>
#include <QMetaType>
#include <QList>
#include <QPair>

typedef QPair<QString, QColor> KColorComboItemDataType;

Q_DECLARE_METATYPE(KColorComboItemDataType)

class KColorCombo : public QComboBox
{
    Q_OBJECT

public:
    explicit KColorCombo(QWidget *parent = 0);
    ~KColorCombo();

    void addColor(const QColor &color, const QString &fileForData);

protected:
    virtual void paintEvent(QPaintEvent *event);
};

#endif	// KCOLORCOMBO_H
