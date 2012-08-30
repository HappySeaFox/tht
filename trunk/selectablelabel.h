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

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual bool eventFilter(QObject *obj, QEvent *event);
    virtual void paintEvent(QPaintEvent *);

signals:
    void selected(bool);

private:
    bool m_selected;
    bool m_wasPress;
    QLabel *m_pixmap;
    QPoint m_vectorStart, m_vectorEnd;
    QColor m_vectorColor;
};

inline
bool SelectableLabel::selected() const
{
    return m_selected;
}

#endif // SELECTABLELABEL_H
