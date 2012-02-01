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

#include <QxtGlobalShortcut>

#include <QContextMenuEvent>
#include <QGridLayout>
#include <QMessageBox>
#include <QCloseEvent>
#include <QClipboard>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QIcon>
#include <QMenu>

#include <windows.h>
#include <psapi.h>

#include "savescreenshot.h"
#include "regionselect.h"
#include "settings.h"
#include "options.h"
#include "about.h"
#include "list.h"
#include "tht.h"
#include "ui_tht.h"

static const int THT_WINDOW_STARTUP_TIMEOUT = 2000;

THT::THT(QWidget *parent) :
    QWidget(parent, Settings::instance()->onTop() ? Qt::WindowStaysOnTopHint : (Qt::WindowFlags)0),
    ui(new Ui::THT),
    m_running(false)
{
    ui->setupUi(this);

    // NYSE only
    ui->checkNyse->setChecked(Settings::instance()->nyseOnly());

    QIcon icon_quit(":/images/quit.png");
    QIcon icon_screenshot(":/images/screenshot.png");

    // context menu
    m_menu = new QMenu(this);
    m_menu->addAction(QIcon(":/images/options.png"), tr("Options..."), this, SLOT(slotOptions()));
    m_menu->addAction(icon_screenshot, tr("Take screenshot..."), this, SLOT(slotTakeScreenshot()));
    m_menu->addAction(tr("Clear links"), this, SLOT(slotClearLinks()));
    m_menu->addSeparator();
    m_menu->addAction(tr("About THT"), this, SLOT(slotAbout()));
    m_menu->addAction(tr("About Qt"), this, SLOT(slotAboutQt()));
    m_menu->addSeparator();
    m_menu->addAction(icon_quit, tr("Quit"), qApp, SLOT(quit()));

    m_timerCheckActive = new QTimer(this);
    m_timerCheckActive->setSingleShot(true);
    m_timerCheckActive->setInterval(50);
    connect(m_timerCheckActive, SIGNAL(timeout()), this, SLOT(slotCheckActive()));

    m_timerLoadToNextWindow = new QTimer(this);
    m_timerLoadToNextWindow->setSingleShot(true);
    m_timerLoadToNextWindow->setInterval(0);
    connect(m_timerLoadToNextWindow, SIGNAL(timeout()), this, SLOT(slotLoadToNextWindow()));

    // layout
    m_layout = new QGridLayout(ui->widgetTickers);
    m_layout->setContentsMargins(0, 0, 0, 0);
    ui->widgetTickers->setLayout(m_layout);

    rebuildUi();

    // restore geometry
    if(Settings::instance()->saveGeometry())
    {
        QSize sz = Settings::instance()->windowSize();

        if(sz.isValid())
            resize(sz);

        QPoint pt = Settings::instance()->windowPosition();

        if(!pt.isNull())
            move(pt);
    }

    // tray icon
    m_tray = new QSystemTrayIcon(QIcon(":/images/chart.png"), this);
    QMenu *trayMenu = new QMenu(this);

    //trayMenu->addAction(tr("Restore"), this, SLOT(activate()));
    trayMenu->addAction(icon_screenshot, tr("Take screenshot..."), this, SLOT(slotTakeScreenshot()));
    trayMenu->addSeparator();
    trayMenu->addAction(icon_quit, tr("Quit"), qApp, SLOT(quit()));

    connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slotTrayActivated(QSystemTrayIcon::ActivationReason)));

    m_tray->setContextMenu(trayMenu);
    m_tray->setVisible(Settings::instance()->hideToTray());

    // global shortcuts
    QxtGlobalShortcut *takeScreen = new QxtGlobalShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_S), this);
    connect(takeScreen, SIGNAL(activated()), this, SLOT(slotTakeScreenshot()));

    checkWindows();
}

THT::~THT()
{
    Settings::instance()->setNyseOnly(ui->checkNyse->isChecked());

    if(Settings::instance()->saveGeometry())
    {
        Settings::instance()->setWindowSize(size());
        Settings::instance()->setWindowPosition(pos());
    }

    delete ui;
}

void THT::closeEvent(QCloseEvent *e)
{
    if(Settings::instance()->hideToTray())
    {
        if(!Settings::instance()->trayNoticeSeen())
        {
            Settings::instance()->setTrayNoticeSeen(true);
            m_tray->showMessage(tr("Notice"), tr("THT will continue to run in a system tray"), QSystemTrayIcon::Information, 7000);
        }

        e->ignore();
        hide();
    }
    else
    {
        e->accept();
        qApp->quit();
    }
}

void THT::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
    m_menu->exec(event->globalPos());
}

void THT::sendKey(int key, bool extended) const
{
    KEYBDINPUT kb = {0};
    INPUT input[2] = {{0}, {0}};

    SHORT vkey = VkKeyScan(key);

    // key down
    if(extended)
        kb.dwFlags = KEYEVENTF_EXTENDEDKEY;

    kb.wVk = vkey;
    kb.wScan = MapVirtualKey(vkey, 0);

    input[0].type = INPUT_KEYBOARD;
    input[0].ki = kb;

    // key up
    kb.dwFlags = KEYEVENTF_KEYUP;

    if(extended)
        kb.dwFlags |= KEYEVENTF_EXTENDEDKEY;

    input[1].type = INPUT_KEYBOARD;
    input[1].ki = kb;

    // send both combinations
    SendInput(2, input, sizeof(INPUT));
}

void THT::sendString(const QString &ticker) const
{
    if(ticker.isEmpty())
    {
        qDebug("THT: Ticker is empty, nothing to send");
        return;
    }

    for(int i = 0;i < ticker.length();i++)
        sendKey(ticker.at(i).toAscii());

    sendKey(VK_RETURN);
}

void THT::rebuildUi()
{
    // check the number of lists
    int nlists = Settings::instance()->numberOfLists();

    // create more
    if(nlists > m_lists.size())
    {
        while(nlists > m_lists.size())
        {
            List *list = new List(m_lists.size()+1, this);

            connect(list, SIGNAL(moveLeft(const QString &)), this, SLOT(slotMoveLeft(const QString &)));
            connect(list, SIGNAL(moveRight(const QString &)), this, SLOT(slotMoveRight(const QString &)));
            connect(list, SIGNAL(loadTicker(const QString &)), this, SLOT(slotLoadTicker(const QString &)));

            m_layout->addWidget(list, 0, m_lists.size());
            m_lists.append(list);
        }
    }
    // delete useless
    else if(nlists < m_lists.size())
    {
        while(nlists < m_lists.size())
        {
            Settings::instance()->removeTickers(m_lists.size());

            delete m_lists.last();
            m_lists.removeLast();
        }
    }

    bool saveTickers = Settings::instance()->saveTickers();

    foreach(List *l, m_lists)
    {
        l->setSaveTickers(saveTickers);
    }
}

void THT::checkWindows()
{
    RECT rect;

    QList<Link>::iterator itEnd = m_windows.end();

    for(QList<Link>::iterator it = m_windows.begin();it != itEnd;++it)
    {
        // remove dead windows
        if(!GetWindowRect((*it).hwnd, &rect))
        {
            qDebug("THT: Window id %d is not valid, removing (%ld)", (int)(*it).hwnd, GetLastError());
            it = --m_windows.erase(it);
        }
        else
        {
            // try to determine their type
            if((*it).type == LinkTypeNotInitialized)
            {
                DWORD dwProcessId;

                GetWindowThreadProcessId((*it).hwnd, &dwProcessId);
                HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);

                if(!h)
                {
                    qDebug("THT: Cannot open process %ld", dwProcessId);
                    continue;
                }

                TCHAR name[MAX_PATH];

                if(!GetProcessImageFileName(h, name, sizeof(name)))
                {
                    qDebug("THT: Cannot get process info %ld (%ld)", dwProcessId, GetLastError());
                    continue;
                }

                QString sname = QFileInfo(
#ifdef UNICODE
                QString::fromWCharArray(name)
#else
                QString::fromUtf8(name)
#endif
                ).fileName().toLower();

                qDebug("THT: Process name of %d is \"%s\"", (int)(*it).hwnd, sname.toAscii().constData());

                if(sname == "advancedget.exe")
                    (*it).type = LinkTypeAdvancedGet;
                else if(sname == "graybox.exe")
                    (*it).type = LinkTypeGraybox;
                else
                    (*it).type = LinkTypeOther;
            }
        }
    }

    // join window types to a status string
    int ag = 0, gb = 0, o = 0;

    for(QList<Link>::iterator it = m_windows.begin();it != itEnd;++it)
    {
        if((*it).type == LinkTypeAdvancedGet)
            ag++;
        else if((*it).type == LinkTypeGraybox)
            gb++;
        else if((*it).type == LinkTypeOther)
            o++;
    }

    ui->labelAG->setNum(ag);
    ui->labelGB->setNum(gb);
    ui->labelO->setNum(o);
}

void THT::loadNextWindow()
{
    m_currentWindow++;

    if(m_currentWindow >= m_windows.size())
    {
        qDebug("THT: Done for all windows");
        busy(false);
        activate();
        m_running = false;
    }
    else
        m_timerLoadToNextWindow->start();
}

void THT::busy(bool b)
{
    foreach(List *l, m_lists)
        l->setIgnoreInput(b);

    ui->stackBusy->setCurrentIndex(b);
}

void THT::activate()
{
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    show();
    activateWindow();
    raise();
}

void THT::slotCheckActive()
{
    if(m_windows.isEmpty())
    {
        qDebug("THT: Window list is empty");
        m_running = false;
        return;
    }

    HWND window = m_windows.at(m_currentWindow).hwnd;
    WINDOWINFO pwi = {0};
    pwi.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(window, &pwi);

    if(GetForegroundWindow() == window && pwi.dwWindowStatus == WS_ACTIVECAPTION)
    {
        qDebug("THT: Found window, sending data");

        QString add;

        if(m_windows.at(m_currentWindow).type == LinkTypeAdvancedGet && ui->checkNyse->isChecked())
            add = "=N";

        sendString(m_ticker + add);
        loadNextWindow();
    }
    else
    {
        if(QDateTime::currentMSecsSinceEpoch() - m_startupTime > THT_WINDOW_STARTUP_TIMEOUT)
        {
            qDebug("THT: Failed to wait the window due to timeout");
            loadNextWindow();
        }
        else
        {
            qDebug("THT: Cannot find window, continuing to search");
            m_timerLoadToNextWindow->start(m_loadToNextInterval);
        }
    }
}

void THT::slotAbout()
{
    About about(this);
    about.exec();
}

void THT::slotAboutQt()
{
    QMessageBox::aboutQt(this);
}

void THT::slotOptions()
{
    Options opt(this);

    if(opt.exec() == QDialog::Accepted)
    {
        opt.save();
        rebuildUi();

        // always on top?
        Qt::WindowFlags flags = windowFlags();

        if(Settings::instance()->onTop())
            flags |= Qt::WindowStaysOnTopHint;
        else
            flags &= ~Qt::WindowStaysOnTopHint;

        setWindowFlags(flags);
        show();

        m_tray->setVisible(Settings::instance()->hideToTray());
    }
}

void THT::slotMoveLeft(const QString &ticker)
{
    qDebug("THT: Move ticker \"%s\" left", qPrintable(ticker));

    int index = m_lists.indexOf(qobject_cast<List *>(sender()));

    if(index < 0)
    {
        qWarning("THT: Cannot find where to move the ticker");
        return;
    }

    if(!index)
    {
        qWarning("THT: Cannot move from the first list");
        return;
    }

    m_lists[--index]->addTicker(ticker);
}

void THT::slotMoveRight(const QString &ticker)
{
    qDebug("THT: Move ticker \"%s\" right", qPrintable(ticker));

    int index = m_lists.indexOf(qobject_cast<List *>(sender()));

    if(index < 0)
    {
        qWarning("THT: Cannot find where to move the ticker");
        return;
    }

    if(index == m_lists.size()-1)
    {
        qWarning("THT: Cannot move from the last list");
        return;
    }

    m_lists[++index]->addTicker(ticker);
}

void THT::slotLoadTicker(const QString &ticker)
{
    qDebug("THT: Load ticker \"%s\"", qPrintable(ticker));

    if(m_running)
    {
        qDebug("THT: In progress, won't load new ticker");
        return;
    }

    checkWindows();

    if(m_windows.isEmpty())
    {
        qDebug("THT: No windows configured");
        m_running = false;
        return;
    }

    m_currentWindow = 0;
    m_running = true;
    m_ticker = ticker;
    m_startupTime = QDateTime::currentMSecsSinceEpoch();

    busy(true);

    m_timerLoadToNextWindow->start();
}

void THT::slotLoadToNextWindow()
{
    HWND window = m_windows.at(m_currentWindow).hwnd;

    qDebug("THT: Trying window %d", (int)window);

    // window flags to set
    int flags = SW_SHOWNORMAL;

    if(IsZoomed(window))
        flags |= SW_SHOWMAXIMIZED;

    m_loadToNextInterval = IsIconic(window) ? 50 : 20;

    // try to switch to this window
    ShowWindow(window, flags);
    SetForegroundWindow(window);

    m_timerCheckActive->start();
}

void THT::slotTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::Trigger:
        {
            if(!isVisible() || isMinimized())
                activate();
            else
                hide();
        }
        break;

        default: ;
    }
}

void THT::slotTakeScreenshot()
{
    bool vis = isVisible();

    hide();

    RegionSelect selector;
    QPixmap px;

    // ignore screenshot
    if(selector.exec() != QDialog::Accepted)
    {
        if(vis)
            activate();

        return;
    }

    px = selector.selection();

    if(px.isNull())
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot take screenshot"));

        if(vis)
            activate();

        return;
    }

    if(vis)
        activate();

    // save screenshot
    SaveScreenshot s(this);

    if(s.exec() == QDialog::Accepted)
    {
        if(s.destination() == SaveScreenshot::SaveScreenshotToClipboard)
            QApplication::clipboard()->setPixmap(px);
        else
        {
            QString fileName = s.fileName();

            if(!fileName.isEmpty())
            {
                if(px.save(fileName))
                    qDebug("THT: Screenshot has been saved to \"%s\"", qPrintable(fileName));
                else
                {
                    QMessageBox::critical(this, tr("Error"), tr("Cannot save screenshot"));
                    qDebug("THT: Cannot save screenshot");
                }
            }
        }
    }
}

void THT::slotClearLinks()
{
    qDebug("THT: Clear links");

    m_windows.clear();
    checkWindows();
}

void THT::slotTargetDropped(const QPoint &p)
{
    POINT pnt;

    pnt.x = p.x();
    pnt.y = p.y();

    HWND hwnd = RealChildWindowFromPoint(GetDesktopWindow(), pnt);

    if(!hwnd)
    {
        qDebug("THT: Cannot find window under cursor %d,%d", p.x(), p.y());
        return;
    }

    // this window
    if(hwnd == winId())
    {
        qDebug("THT: Ignoring ourselves");
        return;
    }

    // desktop window?
    HWND hwnd2 = WindowFromPoint(pnt);

    bool isDesktop = false;
    TCHAR classname[256];

    if(!GetClassName(hwnd2, classname, sizeof(classname)))
    {
        qDebug("THT: Cannot get class name for window %d", (int)hwnd);
    }
    else
    {
        isDesktop = !lstrcmp(classname,
#ifdef UNICODE
         L"SysListView32"
#else
        "SysListView32"
#endif
         );
    }

    // desktop
    if(hwnd2 == GetDesktopWindow() || isDesktop)
    {
        qDebug("THT: Ignoring desktop window");
        return;
    }

    // already linked?
    QList<Link>::iterator itEnd = m_windows.end();

    for(QList<Link>::iterator it = m_windows.begin();it != itEnd;++it)
    {
        if((*it).hwnd == hwnd)
        {
            qDebug("THT: Window %d is already linked", (int)hwnd);
            return;
        }
    }

    qDebug("THT: Window under cursor %d", (int)hwnd);

    // beep
    MessageBeep(MB_OK);

    m_windows.append(Link(hwnd));

    checkWindows();
}

// IPC message
void THT::slotMessageReceived(const QString &msg)
{
    qDebug("THT: Got message \"%s\"", qPrintable(msg));

    if(msg == "wake up")
        activate();
}
