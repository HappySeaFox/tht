/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)
    Copyright (C) 2007 Pino Toscano (pino@kde.org)
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

#include "kcolorcombo.h"

#include <QAbstractItemDelegate>
#include <QApplication>
#include <QStylePainter>

class KColorComboDelegate : public QAbstractItemDelegate
{
    public:
        enum ItemRoles {
            ColorRole = Qt::UserRole + 1
        };

        enum LayoutMetrics {
            FrameMargin = 3
        };

        KColorComboDelegate(QObject *parent = 0);
        virtual ~KColorComboDelegate();

        virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

static QBrush k_colorcombodelegate_brush(const QModelIndex &index, int role)
{
    QBrush brush;
    QVariant v = index.data(role);
    if (v.type() == QVariant::Brush) {
        brush = v.value<QBrush>();
    } else if (v.type() == QVariant::Color) {
        brush = QBrush(v.value<QColor>());
    }
    return brush;
}

KColorComboDelegate::KColorComboDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
}

KColorComboDelegate::~KColorComboDelegate()
{
}

void KColorComboDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // background
    QColor innercolor;
    bool isSelected = (option.state & QStyle::State_Selected);
    bool paletteBrush = (k_colorcombodelegate_brush(index, Qt::BackgroundRole).style() == Qt::NoBrush);
    if (isSelected) {
        innercolor = option.palette.color(QPalette::Highlight);
    } else {
        innercolor = option.palette.color(QPalette::Base);
    }
    // highlight selected item
    QStyleOptionViewItemV4 opt(option);
    opt.showDecorationSelected = true;
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
    QRect innerrect = option.rect.adjusted(FrameMargin, FrameMargin, -FrameMargin, -FrameMargin);
    // inner color
    QVariant cv = index.data(ColorRole);
    if (cv.type() == QVariant::Color) {
        QColor tmpcolor = cv.value<QColor>();
        if (tmpcolor.isValid()) {
            innercolor = tmpcolor;
            paletteBrush = false;
            painter->setPen(Qt::transparent);
            painter->setBrush(innercolor);
            QPainter::RenderHints tmpHint = painter->renderHints();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->drawRoundedRect(innerrect, 2, 2);
            painter->setRenderHints(tmpHint);
            painter->setBrush(Qt::NoBrush);
        }
    }
    // text
    QVariant tv = index.data(Qt::DisplayRole);
    if (tv.type() == QVariant::String) {
        QString text = tv.toString();
        QColor textColor;
        if (paletteBrush) {
            if (isSelected) {
                textColor = option.palette.color(QPalette::Text);
            } else {
                textColor = option.palette.color(QPalette::Text);
            }
        } else {
            int unused, v;
            innercolor.getHsv(&unused, &unused, &v);
            if (v > 128) {
                textColor = Qt::black;
            } else {
                textColor = Qt::white;
            }
        }
        painter->setPen(textColor);
        painter->drawText(innerrect.adjusted(1, 1, -1, -1), Qt::AlignVCenter, text);
    }
}

QSize KColorComboDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // the width does not matter, as the view will always use the maximum width available
    return QSize(100,
                 option.fontMetrics.height()
                    + (index.data(Qt::UserRole).canConvert<KColorComboItemDataType>() ? 14 : 2*FrameMargin));
}

KColorCombo::KColorCombo( QWidget *parent )
    : QComboBox(parent)
{
    setItemDelegate(new KColorComboDelegate(this));
    setMaxVisibleItems(9);
}

KColorCombo::~KColorCombo()
{}

void KColorCombo::addColor(const QColor &color, const QString &fileForData)
{
    addItem(QString("#%1").arg(count()), QVariant::fromValue(KColorComboItemDataType(fileForData, color)));
    setItemData(count()-1, color, KColorComboDelegate::ColorRole);
}

void KColorCombo::paintEvent(QPaintEvent *event)
{
    const QVariant var = itemData(currentIndex());

    if(!var.canConvert<KColorComboItemDataType>())
        QComboBox::paintEvent(event);
    else
    {
        QStylePainter painter(this);
        painter.setPen(palette().color(QPalette::Text));

        // style
        QStyleOptionComboBox opt;
        initStyleOption(&opt);
        painter.drawComplexControl(QStyle::CC_ComboBox, opt);

        // color rectangle
        QRect frame = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::transparent);
        painter.setBrush(QBrush(var.value<KColorComboItemDataType>().second));
        painter.drawRoundedRect(frame.adjusted(1, 1, -1, -1), 2, 2);

        // text
        painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
    }
}
