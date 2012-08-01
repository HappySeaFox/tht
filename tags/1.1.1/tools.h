#ifndef TOOLS_H
#define TOOLS_H

class QWidget;
class QPoint;

class Tools
{
public:
    static void moveWindow(QWidget *w, const QPoint &pt);
};

#endif // TOOLS_H
