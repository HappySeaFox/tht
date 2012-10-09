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
#include <QPointer>
#include <QWidget>
#include <QString>
#include <QList>
#include <QHash>
#include <QMap>
#include <Qt>

#include "tickerneighbors.h"
#include "listitem.h"

#include <windows.h>

class QxtGlobalShortcut;

class QGridLayout;
class QPoint;
class QTimer;
class QMenu;

class List;

namespace Ui
{
    class THT;
}

class THT : public QWidget
{
    Q_OBJECT

public:
    explicit THT(QWidget *parent = 0);
    ~THT();

    virtual void setVisible(bool);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void closeEvent(QCloseEvent *e);
    virtual bool eventFilter(QObject *o, QEvent *e);

private:
    enum LinkType { LinkTypeNotInitialized,
                    LinkTypeAdvancedGet,
                    LinkTypeGraybox,
                    LinkTypeTwinkorswim,
                    LinkTypeMBTDesktop,
                    LinkTypeMBTDesktopPro,
                    LinkTypeOther };

    struct Link
    {
        Link(HWND h = (HWND)0, LinkType lt = LinkTypeNotInitialized)
        {
            hwnd = h;
            type = lt;
            processId = 0;
            threadId = 0;
            findSubControl = 0;
            waitForCaption = true; // true by default
            cachedSubControl = 0;
        }

        HWND hwnd;
        QPoint dropPoint;
        LinkType type;
        DWORD processId;
        DWORD threadId;
        bool waitForCaption;
        HWND cachedSubControl;

        // TODO Do we need to redesign the linking process?
        HWND (*findSubControl)(HWND parent);
    };

    typedef QHash<LinkType, QString> PredefinedTickerMappings;

    struct PredefinedTicker
    {
        PredefinedTicker(const Qt::Key &_key, const PredefinedTickerMappings &_mappings = PredefinedTickerMappings())
            : key(_key),
              mappings(_mappings)
        {}

        Qt::Key key;
        PredefinedTickerMappings mappings;
    };

    void sendKey(int key, bool extended = false);
    void sendString(const QString &str, LinkType = LinkTypeOther);
    void rebuildUi();
    Link checkWindow(HWND hwnd);
    Link checkTargetWindow(const QPoint &, bool allowThisWindow);
    void checkWindows();
    void nextLoadableWindowIndex(int startFrom = 0);
    void loadNextWindow();
    void busy(bool);
    void loadTicker(const QString &);
    void startDelayedScreenshot(bool);
    bool setForeignFocus(HWND window, DWORD threadId);
    void rebuildLinkPoints();
    void raiseWindow(QWidget *);

    static HWND grayBoxFindSubControl(HWND);
    static HWND mbtFindSubControl(HWND);
    static HWND mbtProFindSubControl(HWND);

public slots:
    void activate();

private slots:
    void slotAdjustSize();
    void slotCheckActive();
    void slotAbout();
    void slotAboutQt();
    void slotQuit();
    void slotOptions();
    void slotCopyLeft(const QString &ticker, ListItem::Priority);
    void slotCopyRight(const QString &ticker, ListItem::Priority);
    void slotCopyTo(const QString &, ListItem::Priority, int);
    void slotLoadTicker(const QString &ticker);
    void slotLoadTicker();
    void slotLoadToNextWindow();
    void slotTrayActivated(QSystemTrayIcon::ActivationReason);
    void slotTakeScreenshot();
    void slotTakeScreenshotReal();
    void slotTakeScreenshotFromGlobal();
    void slotClearLists();
    void slotClearLinks();
    void slotManageLinks();
    void slotLoadLinks();
    void slotLockLinks();
    void slotTargetDropped(const QPoint &);
    void slotTickerDropped(const QString &, ListItem::Priority, const QPoint &);
    void slotMessageReceived(const QString &);
    void slotLoadPredefinedTicker();
    void slotOpenOrCloseSearchTicker();
    void slotShowNeighbors(const QString &);
    void slotNeedRebuildFinvizMenu();
    void slotFoolsDay();

private:
    Ui::THT *ui;

    QList<Link> m_windowsLoad;
    QList<Link> m_windowsDrop; // really only one window
    QList<Link> *m_windows;
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
    bool m_useKeyboardInRegion;
    bool m_locked;
    QWidget *m_lastActiveWindow;
    typedef QMap<QString, PredefinedTicker> PredefinedTickers;
    PredefinedTickers m_predefined;
    QPointer<TickerNeighbors> m_sectors;
    QxtGlobalShortcut *m_takeScreen;
};

#endif // THT_H
