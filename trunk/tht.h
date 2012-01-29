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

#ifndef THT_H
#define THT_H

#include <QWidget>
#include <QList>

#include <windows.h>

namespace Ui
{
    class THT;
}

class QTimer;
class QMenu;

class THT : public QWidget
{
    Q_OBJECT
    
public:
    explicit THT(QWidget *parent = 0);
    ~THT();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    void sendString(const QString &str) const;

private slots:
    void slotTimer();
    void slotCheckActive();
    void slotAbout();
    void slotAboutQt();
    void slotOptions();

private:
    Ui::THT *ui;
    QList<HWND> m_windows;
    bool m_running;
    QString m_ticker;
    QTimer *m_timerCheckActive;
    QMenu *m_menu;
};

#endif // THT_H
