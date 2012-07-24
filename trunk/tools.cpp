#include <QDesktopWidget>
#include <QApplication>
#include <QWidget>
#include <QPoint>
#include <QRect>

#include "tools.h"

void Tools::moveWindow(QWidget *w, const QPoint &pt)
{
    if(!w || pt.isNull())
        return;

    const int sn = qApp->desktop()->screenCount();

    for(int screen = 0;screen < sn;screen++)
    {
        QRect dr = qApp->desktop()->availableGeometry(screen);
        QRect headGeometry = QRect(pt, QSize(w->width(), 20));

        // move to a valid position
        if(dr.contains(headGeometry.topLeft()) || dr.contains(headGeometry.bottomRight()))
        {
            w->move(pt);
            break;
        }
    }
}
