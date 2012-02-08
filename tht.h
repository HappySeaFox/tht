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

#include <QSystemTrayIcon>
#include <QWidget>
#include <QList>

#include <windows.h>

namespace Ui
{
    class THT;
}

class QGridLayout;
class QPoint;
class QTimer;
class QMenu;

class List;

class THT : public QWidget
{
    Q_OBJECT
    
public:
    explicit THT(QWidget *parent = 0);
    ~THT();

private:
    enum LinkType { LinkTypeNotInitialized, LinkTypeAdvancedGet, LinkTypeGraybox, LinkTypeOther };

    struct Link
    {
        Link(HWND h = (HWND)0, LinkType lt = LinkTypeNotInitialized)
        {
            hwnd = h;
            type = lt;
        }

        HWND hwnd;
        LinkType type;
    };

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void closeEvent(QCloseEvent *e);

private:
    void sendKey(int key, bool extended = false);
    void sendString(const QString &str);
    void rebuildUi();
    Link checkWindow(HWND hwnd);
    void checkWindows();
    void nextLoadableWindowIndex(int startFrom = 0);
    void loadNextWindow();
    void busy(bool);
    void startDelayedScreenshot(bool);

public slots:
    void activate();

private slots:
    void slotCheckActive();
    void slotAbout();
    void slotAboutQt();
    void slotOptions();
    void slotCopyLeft(const QString &ticker);
    void slotCopyRight(const QString &ticker);
    void slotCopyTo(const QString &, int);
    void slotLoadTicker(const QString &ticker);
    void slotLoadToNextWindow();
    void slotTrayActivated(QSystemTrayIcon::ActivationReason);
    void slotTakeScreenshot();
    void slotTakeScreenshotReal();
    void slotTakeScreenshotFromGlobal();
    void slotClearLists();
    void slotClearLinks();
    void slotTargetDropped(const QPoint &);
    void slotMessageReceived(const QString &);

private:
    Ui::THT *ui;

    QList<Link> m_windows;
    int m_currentWindow;
    bool m_running;
    QTimer *m_timerCheckActive;
    QTimer *m_timerLoadToNextWindow;
    qint64 m_startupTime;
    QMenu *m_menu;
    QList<List *> m_lists;
    QGridLayout *m_layout;
    QString m_ticker;
    QSystemTrayIcon *m_tray;
    bool m_wasVisible;
    KEYBDINPUT m_kbInput;
    INPUT m_input[4];
    bool m_useKeyboardInRegion;
};

#endif // THT_H
