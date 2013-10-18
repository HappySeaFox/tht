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

#ifndef TARGET_H
#define TARGET_H

#include <QPixmap>
#include <QWidget>

class QLabel;

class NumericLabel;

class Target : public QWidget
{
    Q_OBJECT

public:
    explicit Target(QWidget *parent = 0);

    bool mayBeMaster() const;

    void setNumberOfLinks(uint n);

    void setNumberToolTip(const QString &);

    void locked(bool);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual bool eventFilter(QObject *o, QEvent *e);

signals:
    void moving(const QPoint &);
    void dropped(const QPoint &);
    void cancelled();
    void middleClick();
    
private:
    QLabel *m_label;
    NumericLabel *m_number;
    bool m_dragging;
    QPixmap m_drag_black, m_drag_red;
};

#endif // TARGET_H
