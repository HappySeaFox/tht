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

#ifndef IDLETIMER_H
#define IDLETIMER_H

#include <QObject>

class QTimer;

class IdleTimer : public QObject
{
    Q_OBJECT

public:
    explicit IdleTimer(QObject *parent = 0);

    void setInterval(int msec);
    void setEnabled(bool enab);

protected:
    virtual bool eventFilter(QObject *o, QEvent *e);

signals:
    void idle();

private:
    QTimer *m_timer;
    bool m_enabled;
};

#endif // IDLETIMER_H
