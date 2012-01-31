#ifndef TARGET_H
#define TARGET_H

#include <QLabel>

class Target : public QLabel
{
    Q_OBJECT

public:
    explicit Target(QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void dropped(const QPoint &);
    
public slots:
    
};

#endif // TARGET_H
