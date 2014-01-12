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

#include <QWidget>

class QLabel;

class NumericLabel;

#define THT_TARGET_DEFAULT_STYLESHEET \
"QLabel#target               { background: url(:/images/drag.png) center no-repeat; }" \
"QLabel#target[alt=\"true\"] { background: url(:/images/drag_red.png) center no-repeat; }"

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
    virtual void resizeEvent(QResizeEvent *);
    virtual bool eventFilter(QObject *o, QEvent *e);

private:
    void changePixmap(bool alt);
    void resetStyle();

signals:
    void moving(const QPoint &);
    void dropped(const QPoint &);
    void cancelled();
    void middleClick();

private:
    QLabel *m_label;
    NumericLabel *m_number;
    bool m_dragging;
};

#endif // TARGET_H
