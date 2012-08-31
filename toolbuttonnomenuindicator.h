#ifndef TOOLBUTTONNOMENUINDICATOR_H
#define TOOLBUTTONNOMENUINDICATOR_H

#include <QToolButton>

class ToolButtonNoMenuIndicator : public QToolButton
{
public:
    explicit ToolButtonNoMenuIndicator(QWidget *parent = 0);

protected:
    virtual void paintEvent(QPaintEvent *);
};

#endif // TOOLBUTTONNOMENUINDICATOR_H
