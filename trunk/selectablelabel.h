#ifndef SELECTABLELABEL_H
#define SELECTABLELABEL_H

#include <QLabel>
#include <QPoint>
#include <QColor>

class QPixmap;
class QWidget;

class SelectableLabel : public QLabel
{
    Q_OBJECT

public:
    SelectableLabel(const QPixmap &px, const QPoint &startPoint, const QPoint &endPoint, const QColor &cl, QWidget *parent);

    void setSelected(bool, bool loud = true);
    bool selected() const;

    QPoint vectorStart() const;
    QPoint vectorEnd() const;
    QColor vectorColor() const;

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

signals:
    void selected(bool);

private:
    bool m_selected;
    bool m_wasPress;
    QPoint m_vectorStart, m_vectorEnd;
    QColor m_vectorColor;
};

inline
bool SelectableLabel::selected() const
{
    return m_selected;
}

inline
QPoint SelectableLabel::vectorStart() const
{
    return m_vectorStart;
}

inline
QPoint SelectableLabel::vectorEnd() const
{
    return m_vectorEnd;
}

inline
QColor SelectableLabel::vectorColor() const
{
    return m_vectorColor;
}

#endif // SELECTABLELABEL_H
