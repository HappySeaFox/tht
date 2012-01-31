#include <QApplication>
#include <QMouseEvent>
#include <QCursor>
#include <QPixmap>

#include "target.h"

Target::Target(QWidget *parent) :
    QLabel(parent)
{
    setPixmap(QPixmap(":/images/drag.png"));
    setMouseTracking(true);
}

void Target::mousePressEvent(QMouseEvent *event)
{
    qDebug("THT: Start dragging");
    QApplication::setOverrideCursor(QCursor(*pixmap()));
}

void Target::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint p = QCursor::pos();

    qDebug("THT: Dropped at %d,%d", p.x(), p.y());
    QApplication::restoreOverrideCursor();
    event->accept();

    emit dropped(p);
}
