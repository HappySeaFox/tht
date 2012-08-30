#ifndef SELECTABLELABEL_H
#define SELECTABLELABEL_H

#include <QLabel>

class QPixmap;
class QWidget;

class SelectableLabel : public QLabel
{
    Q_OBJECT

public:
    SelectableLabel(const QPixmap &px, QWidget *parent);

    void setSelected(bool, bool loud = true);
    bool selected() const;

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

signals:
    void selected(bool);

private:
    bool m_selected;
    bool m_wasPress;
};

inline
bool SelectableLabel::selected() const
{
    return m_selected;
}

#endif // SELECTABLELABEL_H
