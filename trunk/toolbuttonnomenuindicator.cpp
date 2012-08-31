#include <QStyleOptionToolButton>
#include <QStylePainter>
#include <QPaintEvent>

#include "toolbuttonnomenuindicator.h"

ToolButtonNoMenuIndicator::ToolButtonNoMenuIndicator(QWidget *parent) :
    QToolButton(parent)
{}

void ToolButtonNoMenuIndicator::paintEvent(QPaintEvent *pe)
{
    QStylePainter p(this);
    QStyleOptionToolButton opt;

    p.setClipRect(pe->rect());

    initStyleOption(&opt);

    opt.features &= ~QStyleOptionToolButton::HasMenu;

    p.drawComplexControl(QStyle::CC_ToolButton, opt);
}
