#ifndef SCREENSHOTEDITORWIDGET_H
#define SCREENSHOTEDITORWIDGET_H

#include <QPixmap>
#include <QLabel>
#include <QList>

class QPoint;

class SelectableLabel;

class ScreenshotEditorWidget : public QLabel
{
    Q_OBJECT

public:
    explicit ScreenshotEditorWidget(QWidget *parent = 0);

    void setPixmap(const QPixmap &);

    QPixmap pixmap();

    QList<SelectableLabel *> labels();

    enum EditType { None = -1, Long, Short, Stop, Text };

    EditType editType() const;

    void cancel();

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    SelectableLabel *addLabel(const QPoint &pt, const QPixmap &px = QPixmap());

public slots:
    void startLong();
    void startShort();
    void startStop();
    void startText();
    void deleteSelected();

private slots:
    void slotResetCursor();
    void slotSelected(bool);

signals:
    void selected(SelectableLabel *, bool);

private:
    EditType m_editType;
    QPixmap m_pixmaps[3];
    QPixmap m_textPixmap;
    bool m_wasPress;
};

inline
ScreenshotEditorWidget::EditType ScreenshotEditorWidget::editType() const
{
    return m_editType;
}

#endif // SCREENSHOTEDITORWIDGET_H
