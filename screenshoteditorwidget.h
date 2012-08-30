#ifndef SCREENSHOTEDITORWIDGET_H
#define SCREENSHOTEDITORWIDGET_H

#include <QPixmap>
#include <QPoint>
#include <QLabel>
#include <QColor>
#include <QList>

class SelectableLabel;

class ScreenshotEditorWidget : public QLabel
{
    Q_OBJECT

public:
    explicit ScreenshotEditorWidget(QWidget *parent = 0);

    void setPixmap(const QPixmap &);

    QPixmap pixmap();

    enum EditType { None = -1, Buy, Sail, Stop, Text };

    EditType editType() const;

    void cancel();

    QList<SelectableLabel *> labels() const;

    void restoreLabels();
    void saveLabels();
    void clearLabels();

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *);

private:
    SelectableLabel *addLabel(const QPoint &startPoint, const QPoint &endPoint, const QPixmap &px = QPixmap());

public slots:
    void startBuy();
    void startSell();
    void startStop();
    void startText();
    void deleteSelected();
    void selectAll(bool select = true);

private slots:
    void slotResetCursor();
    void slotSelected(bool);
    void slotDestroyed();

signals:
    void selected(SelectableLabel *, bool);

private:
    EditType m_editType;
    QPixmap m_pixmaps[3];
    QColor m_colors[4];
    QPixmap m_textPixmap;
    bool m_wasPress;
    QList<SelectableLabel *> m_labels, m_savedLabels;
    QPoint m_startPoint, m_currentPoint;
};

inline
QList<SelectableLabel *> ScreenshotEditorWidget::labels() const
{
    return m_labels;
}

inline
ScreenshotEditorWidget::EditType ScreenshotEditorWidget::editType() const
{
    return m_editType;
}

#endif // SCREENSHOTEDITORWIDGET_H
