/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SCREENSHOTEDITORWIDGET_H
#define SCREENSHOTEDITORWIDGET_H

#include <QPixmap>
#include <QPoint>
#include <QLabel>
#include <QColor>
#include <QList>

class QPainter;
class QRect;

class SelectableLabel;

class ScreenshotEditorWidget : public QLabel
{
    Q_OBJECT

public:
    explicit ScreenshotEditorWidget(QWidget *parent = 0);

    void setPixmap(const QPixmap &);

    QPixmap pixmap();

    enum EditType { None = -1, Buy, Sale, Stop, Text, Ellipse };

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
    void drawVector(QPainter *p, const QColor &color, const QPoint &pt1, const QPoint &pt2);
    void drawEllipse(QPainter *p, const QRect &rc);
    void drawArrow(QPainter *p, const QPoint &pt1, const QPoint &pt2);
    void resetCursor();

public slots:
    void startBuy();
    void startSale();
    void startStop();
    void startText();
    void startEllipse();
    void deleteSelected();
    void selectAll(bool select = true);

private slots:
    void slotSelected(bool);
    void slotDestroyed();

signals:
    void selected(SelectableLabel *, bool);

private:
    EditType m_editType;
    QPixmap m_pixmaps[3];
    QColor m_colors[3];
    QColor m_ellipseFillColor;
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
