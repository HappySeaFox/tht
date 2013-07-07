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

#include <QWhatsThisClickedEvent>
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QApplication>
#include <QKeySequence>
#include <QMutexLocker>
#include <QMapIterator>
#include <QGridLayout>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QShortcut>
#include <QDateTime>
#include <QMimeData>
#include <QPalette>
#include <QEvent>
#include <QTimer>
#include <QMutex>
#include <QIcon>
#include <QMenu>
#include <QDate>
#include <QUrl>

#include <qxtglobalshortcut.h>

#include <windows.h>
#include <winnt.h>
#include <psapi.h>

#include "tickersdatabaseupdater.h"
#include "linkpointmanager.h"
#include "masterdataevent.h"
#include "savescreenshot.h"
#include "pluginmanager.h"
#include "regionselect.h"
#include "pluginloader.h"
#include "tickerinput.h"
#include "remotedate.h"
#include "settings.h"
#include "sqltools.h"
#include "options.h"
#include "ticker.h"
#include "about.h"
#include "tools.h"
#include "list.h"
#include "tht.h"
#include "ui_tht.h"

static const int          THT_WINDOW_STARTUP_TIMEOUT = 1200;
static const char * const THT_PRIVATE_TICKER_PREFIX = "=THT=";

static QMutex mutexForWinEventCallback;

static void CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook,
                                          DWORD dwEvent,
                                          HWND hwnd,
                                          LONG idObject,
                                          LONG idChild,
                                          DWORD dwEventThread,
                                          DWORD dwmsEventTime)
{
    QMutexLocker locker(&mutexForWinEventCallback);

    Q_UNUSED(hWinEventHook)
    Q_UNUSED(dwEvent)
    Q_UNUSED(idObject)
    Q_UNUSED(idChild)
    Q_UNUSED(dwEventThread)
    Q_UNUSED(dwmsEventTime)

    TCHAR title[MAX_PATH];

    GetWindowText(hwnd, title, sizeof(title));

    QString stitle =
#ifdef UNICODE
        QString::fromWCharArray(title);
#else
        QString::fromUtf8(title);
#endif

    QRegExp rx("(" + Settings::instance()->tickerValidator().pattern() + ")[\\s,:;]+");

    QString ticker;

    if(!rx.indexIn(stitle))
        ticker = rx.cap(1);

    if(!ticker.isEmpty())
    {
        MasterDataEvent mde(hwnd, ticker);
        QApplication::sendEvent(qApp, &mde);
    }
}

/***************************************/

THT::THT() :
    QWidget(0,
            Qt::Window
            | Qt::WindowMinimizeButtonHint
            | Qt::WindowCloseButtonHint
            | Qt::CustomizeWindowHint),
    ui(new Ui::THT),
    m_windows(&m_windowsLoad),
    m_running(false),
    m_locked(false),
    m_lastActiveWindow(0),
    m_drawnWindow(0),
    m_linksChanged(false),
    m_checkForMaster(MasterPolicyAuto),
    m_wasActive(0)
{
    if(SETTINGS_GET_BOOL(SETTING_ONTOP))
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    ui->setupUi(this);

    qDebug("Registered master data event type: %s",
           (QEvent::registerEventType(THT_MASTER_DATA_EVENT_TYPE) == THT_MASTER_DATA_EVENT_TYPE) ? "yes" : "no");

    // initialize all plugins
    PluginLoader::instance()->init();

    setAcceptDrops(true);

    // NYSE only
    ui->checkNyse->setChecked(SETTINGS_GET_BOOL(SETTING_NYSE_ONLY));

    // global shortcuts
    m_takeScreen = new QxtGlobalShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_S), this);
    connect(m_takeScreen, SIGNAL(activated()), this, SLOT(slotTakeScreenshotFromGlobal()));

    m_restore = new QxtGlobalShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_R), this);
    connect(m_restore, SIGNAL(activated()), this, SLOT(activate()));

    reconfigureGlobalShortcuts();

    QIcon icon_quit(":/images/quit.png");
    QIcon icon_screenshot(":/images/screenshot.png");
    QIcon icon_chart(":/images/chart.ico");

    // context menu
    QShortcut *help_shortcut = new QShortcut(QKeySequence::HelpContents, this, SLOT(slotAbout()));
    QShortcut *quit_shortcut = new QShortcut(Qt::CTRL+Qt::Key_Q, this, SLOT(slotQuit()));
    QShortcut *clear_shortcut = new QShortcut(Qt::ALT+Qt::Key_N, this, SLOT(slotClearLists()));

    m_menu = new QMenu(this);
    m_menu->addAction(QIcon(":/images/options.png"), tr("Options..."), this, SLOT(slotOptions()));
    m_menu->addSeparator();

    m_menu->addAction(icon_screenshot, tr("Take screenshot...") + '\t' + m_takeScreen->shortcut().toString(),
                      this, SLOT(slotTakeScreenshot()));

    //: Infinitive
    m_menu->addAction(tr("Clear links"), this, SLOT(slotClearLinks()));
    m_menu->addSeparator();

    m_menu->addAction(tr("Clear ticker lists") + '\t' + clear_shortcut->key().toString(), this, SLOT(slotClearLists()));
    m_menu->addAction(tr("Load ticker") + "..." + "\tL", this, SLOT(slotLoadTicker()));
    QMenu *menu_load = m_menu->addMenu(tr("Load ticker"));

    m_menu->addSeparator();
    m_menu->addAction(icon_chart, tr("About THT...") + '\t' + help_shortcut->key().toString(), this, SLOT(slotAbout()));
    m_menu->addAction(tr("About Qt..."), this, SLOT(slotAboutQt()));
    m_menu->addAction(tr("Plugins..."), this, SLOT(slotPlugins()));
    m_menu->addSeparator();
    m_menu->addAction(icon_quit, tr("Quit") + '\t' + quit_shortcut->key().toString(), this, SLOT(slotQuit()));

    m_timerCheckActive = new QTimer(this);
    m_timerCheckActive->setSingleShot(true);
    m_timerCheckActive->setInterval(50);
    connect(m_timerCheckActive, SIGNAL(timeout()), this, SLOT(slotCheckActive()));

    m_timerLoadToNextWindow = new QTimer(this);
    m_timerLoadToNextWindow->setSingleShot(true);
    m_timerLoadToNextWindow->setInterval(10);
    connect(m_timerLoadToNextWindow, SIGNAL(timeout()), this, SLOT(slotLoadToNextWindow()));

    // layout
    m_layout = new QGridLayout(ui->widgetTickers);
    m_layout->setContentsMargins(0, 0, 0, 0);
    ui->widgetTickers->setLayout(m_layout);

    rebuildUi();

    QString startupTicker;

    // set focus
    foreach(List *l, m_lists)
    {
        if(l->hasTickers())
        {
            l->initialSelect();
            startupTicker = l->currentTicker();
            break;
        }
    }

    if(startupTicker.isEmpty())
        m_lists.at(0)->setFocus();

    // restore geometry
    if(SETTINGS_GET_BOOL(SETTING_SAVE_GEOMETRY))
    {
        QSize sz = SETTINGS_GET_SIZE(SETTING_SIZE);

        if(sz.isValid())
            resize(sz);

        Tools::moveWindow(this, SETTINGS_GET_POINT(SETTING_POSITION));
    }

    m_tray = new QSystemTrayIcon(icon_chart, this);
    QMenu *trayMenu = new QMenu(this);

    //: Means "Restore from tray"
    trayMenu->addAction(tr("Restore"), this, SLOT(activate()));
    trayMenu->addAction(icon_screenshot, tr("Take screenshot..."), this, SLOT(slotTakeScreenshot()));
    trayMenu->addSeparator();
    trayMenu->addAction(tr("About THT"), this, SLOT(slotAbout()));
    trayMenu->addSeparator();
    trayMenu->addAction(icon_quit, tr("Quit"), qApp, SLOT(quit()));

    connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slotTrayActivated(QSystemTrayIcon::ActivationReason)));

    m_tray->setContextMenu(trayMenu);
    m_tray->setVisible(SETTINGS_GET_BOOL(SETTING_HIDE_TO_TRAY));

    checkWindows();

    // predefined tickers, menu & shortcuts
    PredefinedTickerMappings mappings;

    // NASDAQ Composite
    mappings.insert(LinkTypeThinkorswim, "COMP");
    m_predefined.insert("$COMPQ", PredefinedTicker(Qt::Key_Q, mappings));

    // DOW JONES
    mappings.clear();
    mappings.insert(LinkTypeMBTDesktop,    "$DJI");
    mappings.insert(LinkTypeMBTDesktopPro, "$DJI");
    mappings.insert(LinkTypeThinkorswim,   "DJX");
    m_predefined.insert("$INDU", PredefinedTicker(Qt::Key_I, mappings));

    // S&P 500 Index
    mappings.clear();
    mappings.insert(LinkTypeThinkorswim, "SPX");
    m_predefined.insert("$SPX", PredefinedTicker(Qt::Key_S, mappings));

    // NYSE Volume
    m_predefined.insert("$TVOL", PredefinedTicker(Qt::Key_T));

    // Volatility S&P 500
    mappings.clear();
    mappings.insert(LinkTypeThinkorswim, "VIX");
    m_predefined.insert("$VIX", PredefinedTicker(Qt::Key_V, mappings));

    // S&P 500 Fund
    m_predefined.insert("SPY", PredefinedTicker(Qt::Key_D));

    PredefinedTickers::const_iterator itEnd = m_predefined.end();

    for(PredefinedTickers::const_iterator it = m_predefined.begin();it != itEnd;++it)
    {
        QShortcut *s = new QShortcut(it.value().key, this, SLOT(slotLoadPredefinedTicker()));

        s->setProperty("predefined-ticker-key", it.key());

        menu_load->addAction(it.key() + '\t' + QKeySequence(it.value().key).toString(), s, SIGNAL(activated()));
    }

    // lock links
    new QShortcut(Qt::CTRL+Qt::Key_L, this, SLOT(slotLockLinks()));
    new QShortcut(Qt::Key_L, this, SLOT(slotLoadTicker()));
    new QShortcut(QKeySequence::Find, this, SLOT(slotOpenOrCloseSearchTicker()));

    // db updater
    new TickersDatabaseUpdater(this);

    // sectors
    if(SETTINGS_GET_BOOL(SETTING_RESTORE_NEIGHBORS_AT_STARTUP) && SETTINGS_GET_BOOL(SETTING_SHOW_NEIGHBORS_AT_STARTUP))
        slotShowNeighbors(startupTicker);

    if(!SETTINGS_GET_BOOL(SETTING_FOOLSDAY_SEEN))
        QTimer::singleShot(0, this, SLOT(slotFoolsDay()));

    m_newYorkDate = new RemoteDate("Eastern Standard Time");

    // FOMC warning
    m_timerFomcCheck = new QTimer(this);
    m_timerFomcCheck->setSingleShot(true);
    m_timerFomcCheck->setInterval(60*60*1000); // check every 1 hour
    connect(m_timerFomcCheck, SIGNAL(timeout()), this, SLOT(slotFomcCheck()));

    QTimer::singleShot(0, this, SLOT(slotFomcCheck()));

    // link points
    QMenu *linkPointsMenu = new QMenu(ui->pushLinks);
    ui->pushLinks->setMenu(linkPointsMenu);

    rebuildLinks();
    QTimer::singleShot(0, this, SLOT(slotRestoreLinks()));

    // watch for QWhatsThisClickedEvent
    qApp->installEventFilter(this);
}

THT::~THT()
{
    unhookEverybody();

    SETTINGS_SET_BOOL(SETTING_NYSE_ONLY, ui->checkNyse->isChecked(), Settings::NoSync);

    if(SETTINGS_GET_BOOL(SETTING_SAVE_GEOMETRY))
    {
        SETTINGS_SET_SIZE(SETTING_SIZE, size(), Settings::NoSync);
        SETTINGS_SET_POINT(SETTING_POSITION, pos(), Settings::NoSync);
    }

    if(m_linksChanged && SETTINGS_GET_BOOL(SETTING_RESTORE_LINKS_AT_STARTUP))
    {
        QList<LinkedWindow> list;

        foreach(Link l, m_windowsLoad)
        {
            LinkedWindow lw(l.hook, l.dropPoint);

            qDebug("Saving link point: master(%s), %dx%d", lw.master ? "yes" : "no", lw.point.x(), lw.point.y());

            list.append(lw);
        }

        SETTINGS_SET_LINKED_WINDOWS(SETTING_LAST_LINKS, list);
    }

    SETTINGS_SET_BOOL(SETTING_SHOW_NEIGHBORS_AT_STARTUP, (bool)m_sectors);

    delete m_newYorkDate;
    delete ui;
}

void THT::setVisible(bool vis)
{
    if(!vis)
    {
        m_lastActiveWindow = qApp->activeWindow();

        if(m_sectors)
            m_sectors->hide();
    }

    QWidget::setVisible(vis);
}

void THT::contextMenuEvent(QContextMenuEvent *event)
{
    QApplication::restoreOverrideCursor();

    event->accept();
    m_menu->exec(event->globalPos());
}

void THT::closeEvent(QCloseEvent *e)
{
    if(SETTINGS_GET_BOOL(SETTING_HIDE_TO_TRAY))
    {
        if(!SETTINGS_GET_BOOL(SETTING_TRAY_NOTICE_SEEN))
        {
            SETTINGS_SET_BOOL(SETTING_TRAY_NOTICE_SEEN, true);
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

bool THT::eventFilter(QObject *o, QEvent *e)
{
    QEvent::Type type = e->type();

    if(type == QEvent::WhatsThisClicked)
    {
        QWhatsThisClickedEvent *ce = static_cast<QWhatsThisClickedEvent *>(e);

        if(ce)
        {
            QUrl url(ce->href());

            if(url.isValid())
                QDesktopServices::openUrl(url);
        }

        return true;
    }
    else if(type == THT_MASTER_DATA_EVENT_TYPE)
    {
        MasterDataEvent *mde = static_cast<MasterDataEvent *>(e);

        if(mde)
            masterHasBeenChanged(mde->hwnd(), mde->ticker());
    }

    return QObject::eventFilter(o, e);
}

void THT::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasFormat("text/plain"))
    {
        qDebug("Accepting dragged MIME in main window");
        e->acceptProposedAction();
    }
    else
        e->ignore();
}

void THT::dragMoveEvent(QDragMoveEvent *e)
{
    if(e->mimeData()->hasFormat("text/plain"))
        e->acceptProposedAction();
    else
        e->ignore();
}

void THT::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void THT::dropEvent(QDropEvent *e)
{
    e->acceptProposedAction();

    QString tickers = e->mimeData()->text().trimmed().toUpper();
    QPoint pos = mapToGlobal(e->pos());
    bool added = false;

    foreach(List *l, m_lists)
    {
        if(l->contains(pos))
        {
            qDebug("Dropped onto a list");
            l->addTickers(tickers.split(QRegExp("\\s+"), QString::SkipEmptyParts), List::Fix);
            added = true;
            break;
        }
    }

    if(!added)
    {
        if(Settings::instance()->tickerValidator().exactMatch(tickers))
        {
            qDebug("Dropped onto a main window");
            loadTicker(tickers);
        }
        else
            qDebug("Dropped ticker doesn't match the regexp");
    }
}

void THT::sendKey(int key, bool extended)
{
    KEYBDINPUT kbInput = {0};
    INPUT input[4] = {{0}, {0}, {0}, {0}};

    int nelem = 2;
    int index = 0;

    // do we need SHIFT key? Don't check all the cases, we don't need all of them
    bool shift = (key >= '!' && key <= '+');

    // send SHIFT down
    if(shift)
    {
        nelem += 2;

        kbInput.dwFlags = 0;
        kbInput.wVk = VK_SHIFT;
        kbInput.wScan = MapVirtualKey(VK_SHIFT, 0);

        input[index].type = INPUT_KEYBOARD;
        input[index].ki = kbInput;

        ++index;
    }

    // key down
    SHORT vkey = VkKeyScan(key);

    if(extended)
        kbInput.dwFlags = KEYEVENTF_EXTENDEDKEY;

    kbInput.wVk = vkey;
    kbInput.wScan = MapVirtualKey(vkey, 0);

    input[index].type = INPUT_KEYBOARD;
    input[index].ki = kbInput;
    ++index;

    // key up
    kbInput.dwFlags = KEYEVENTF_KEYUP;

    if(extended)
        kbInput.dwFlags |= KEYEVENTF_EXTENDEDKEY;

    input[index].type = INPUT_KEYBOARD;
    input[index].ki = kbInput;
    ++index;

    // SHIFT up
    if(shift)
    {
        kbInput.dwFlags = KEYEVENTF_KEYUP;
        kbInput.wVk = VK_SHIFT;
        kbInput.wScan = MapVirtualKey(VK_SHIFT, 0);

        input[index].type = INPUT_KEYBOARD;
        input[index].ki = kbInput;
    }

    // send both combinations
    SendInput(nelem, input, sizeof(INPUT));
}

void THT::sendString(const QString &ticker, LinkType type)
{
    if(ticker.isEmpty())
    {
        qDebug("Ticker is empty, nothing to send");
        return;
    }

    for(int i = 0;i < ticker.length();i++)
        sendKey(ticker.at(i).toLatin1());

    // Fix for TOS@paperMoney
    if(type == LinkTypeThinkorswim)
        Sleep(10);

    sendKey(VK_RETURN);
}

void THT::rebuildUi()
{
    bool doResize = true;

    // check the number of lists
    int nlists = Settings::instance()->numberOfLists();

    if(nlists < 1)
        nlists = 1;

    // create more
    if(nlists > m_lists.size())
    {
        while(nlists > m_lists.size())
        {
            List *list = new List(m_lists.size()+1, this);

            connect(list, SIGNAL(copyLeft(Ticker)),
                    this, SLOT(slotCopyLeft(Ticker)));

            connect(list, SIGNAL(copyRight(Ticker)),
                    this, SLOT(slotCopyRight(Ticker)));

            connect(list, SIGNAL(copyTo(Ticker,int)),
                    this, SLOT(slotCopyTo(Ticker,int)));

            connect(list, SIGNAL(loadTicker(QString)),
                    this, SLOT(slotLoadTicker(QString)));

            connect(list, SIGNAL(tickerDropped(Ticker,QPoint)),
                    this, SLOT(slotTickerDropped(Ticker,QPoint)));

            connect(list, SIGNAL(showNeighbors(QString)),
                    this, SLOT(slotShowNeighbors(QString)));

            connect(list, SIGNAL(tickerMoving(QPoint)),
                    this, SLOT(slotTargetMoving(QPoint)));

            connect(list, SIGNAL(tickerCancelled()),
                    this, SLOT(slotTargetCancelled()));

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
    else
        doResize = false;

    bool saveTickers = SETTINGS_GET_BOOL(SETTING_SAVE_TICKERS);
    bool listHeader = SETTINGS_GET_BOOL(SETTING_LIST_HEADER);

    foreach(List *l, m_lists)
    {
        l->setSaveTickers(saveTickers);
        l->showHeader(listHeader);
    }

    if(doResize)
        QTimer::singleShot(0, this, SLOT(slotAdjustSize()));
}

void THT::checkWindow(Link *link)
{
    if(!link)
        return;

    // try to determine type
    link->threadId = GetWindowThreadProcessId(link->hwnd, &link->processId);

    HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, link->processId);

    if(!h)
    {
        qWarning("Cannot open process %ld", link->processId);
        return;
    }

    TCHAR name[MAX_PATH];

    // get executable name
    if(!GetProcessImageFileName(h, name, sizeof(name)))
    {
        qWarning("Cannot get a process info %ld (%ld)", link->processId, GetLastError());
        CloseHandle(h);
        return;
    }

    QString sname = QFileInfo(
#ifdef UNICODE
        QString::fromWCharArray(name)
#else
        QString::fromUtf8(name)
#endif
        ).fileName().toLower();

    QString cname;

    if(!GetClassName(link->hwnd, name, sizeof(name)))
        qWarning("Cannot get a class name for window %p (%ld)", link->hwnd, GetLastError());
    else
        cname =
#ifdef UNICODE
            QString::fromWCharArray(name);
#else
            QString::fromUtf8(name);
#endif

    qDebug("Process name of %p is \"%s\", class name is \"%s\"", link->hwnd,
                                                                    qPrintable(sname),
                                                                    qPrintable(cname));

    if(sname == "advancedget.exe" || sname == "winsig.exe")
        link->type = LinkTypeAdvancedGet;
    else if(sname == "dastrader.exe")
        link->type = LinkTypeDASTraderPro;
    else if(sname == "graybox.exe")
        link->type = LinkTypeGraybox;
    else if(sname == "thinkorswim.exe")
        link->type = LinkTypeThinkorswim;
    else if(sname == "laser.exe")
        link->type = LinkTypeLaser;
    else if(sname == "lightspeed.exe")
        link->type = LinkTypeLightspeed;
    else if((sname == "mbtdes~1.exe" || sname == "mbtdesktop.exe") && cname == "MbtTearFrame")
        link->type = LinkTypeMBTDesktop;
    else if((sname == "mbtdes~1.exe" || sname == "mbtdesktoppro.exe") && cname == "MbtNavPro_FloatFrame")
        link->type = LinkTypeMBTDesktopPro;
    else if(sname == "fusion.exe")
        link->type = LinkTypeFusion;
    else if(sname == "takion.exe")
        link->type = LinkTypeTakion;
    else if(sname == "roxdemo.exe" || sname == "rox.exe")
        link->type = LinkTypeROX;
    else if(sname == "archeclient.exe")
        link->type = LinkTypeArchePro;
    else
        link->type = LinkTypeOther;

    if(link->type != LinkTypeNotInitialized)
        qDebug("Window under cursor is %p", link->hwnd);

    CloseHandle(h);
}

THT::Link THT::checkTargetWindow(const QPoint &p, bool allowThisWindow)
{
    POINT pnt = {0};

    pnt.x = p.x();
    pnt.y = p.y();

    HWND hwnd = RealChildWindowFromPoint(GetDesktopWindow(), pnt);

    if(!IsWindow(hwnd))
    {
        qDebug("Cannot find window under cursor %d,%d", p.x(), p.y());
        return Link();
    }

    // this window
    if(!allowThisWindow && hwnd == reinterpret_cast<HWND>(winId()))
    {
        qDebug("Ignoring ourselves");
        return Link();
    }

    // desktop
    if(Tools::isDesktop(hwnd))
    {
        qDebug("Ignoring desktop window");
        return Link();
    }

    // already linked?
    QList<Link>::iterator itEnd = m_windows->end();

    for(QList<Link>::iterator it = m_windows->begin();it != itEnd;++it)
    {
        if((*it).hwnd == hwnd)
        {
            qDebug("Window %p is already linked", hwnd);
            return Link();
        }
    }

    Link link = Link(hwnd);

    link.subControl = Tools::RealWindowFromPoint(pnt);

    if(link.subControl == link.hwnd)
    {
        qDebug("Subcontrol is the same top-level window, setting to 0");
        link.subControl = 0;
    }
    else if(GetAncestor(link.subControl, GA_ROOT) != hwnd)
    {
        qDebug("Subcontrol belongs to another top-level window, setting to 0");
        link.subControl = 0;
    }

    if(link.subControl)
    {
        TCHAR name[256];

        if(!GetClassName(link.subControl, name, sizeof(name)))
            qWarning("Cannot get a class name for subcontrol %p (%ld)", link.subControl, GetLastError());
        else if(!lstrcmp(name, TEXT("Edit")))
            link.subControlSupportsClearing = true;
    }

    qDebug("Subcontrol: %p", link.subControl);

    return link;
}

void THT::checkWindows()
{
    QString tooltip = "<table>";

    QList<Link>::iterator itEnd = m_windows->end();

    for(QList<Link>::iterator it = m_windows->begin();it != itEnd;)
    {
        // remove dead windows
        if(!IsWindow((*it).hwnd))
        {
            qDebug("Window id %p is not valid, removing", (*it).hwnd);
            it = m_windows->erase(it);
            itEnd = m_windows->end();
        }
        else
            ++it;
    }

    if(m_windows == &m_windowsDrop)
        return;

    if(m_windows->isEmpty())
    {
        ui->stackLinks->setCurrentIndex(0); // "No links" warning
        return;
    }

    ui->labelLinks->setNum(m_windows->size());
    ui->stackLinks->setCurrentIndex(1);

    QMap<QString, int> mappings;
    int others = 0;

    // construct the tooltip
    for(QList<Link>::iterator it = m_windows->begin();it != itEnd;++it)
    {
        if((*it).type == LinkTypeAdvancedGet)
            mappings["Advaced Get"]++;
        else if((*it).type == LinkTypeArchePro)
            mappings["Arche Pro"]++;
        else if((*it).type == LinkTypeDASTraderPro)
            mappings["DAS Trader Pro"]++;
        else if((*it).type == LinkTypeFusion)
            mappings["Fusion"]++;
        else if((*it).type == LinkTypeLaser)
            mappings["Laser Trade"]++;
        else if((*it).type == LinkTypeLightspeed)
            mappings["Lightspeed Trader"]++;
        else if((*it).type == LinkTypeGraybox)
            mappings["Graybox"]++;
        else if((*it).type == LinkTypeMBTDesktop)
            mappings["MBT Desktop"]++;
        else if((*it).type == LinkTypeMBTDesktopPro)
            mappings["MBT Desktop Pro"]++;
        else if((*it).type == LinkTypeROX)
            mappings["ROX"]++;
        else if((*it).type == LinkTypeTakion)
            mappings["Takion"]++;
        else if((*it).type == LinkTypeThinkorswim)
            mappings["Thinkorswim"]++;
        else
            others++;
    }

    QMapIterator<QString, int> i(mappings);

    while(i.hasNext())
    {
        i.next();
        tooltip += QString("<tr><td>%1:</td><td>%2</td></tr>").arg(i.key()).arg(i.value());
    }

    if(others)
        tooltip += QString("<tr><td>%1:</td><td>%2</td></tr>").arg(tr("Unknown")).arg(others);

    tooltip += "</table>";
    tooltip.replace(QRegExp("\\s"), "&nbsp;");

    ui->labelLinks->setToolTip(tooltip);
    ui->labelLinks_n->setToolTip(tooltip);
}

void THT::nextLoadableWindowIndex(int delta)
{
    m_currentWindow += delta;

    if(m_ticker.startsWith('$'))
    {
        while(m_currentWindow < m_windows->size() && m_windows->at(m_currentWindow).type != LinkTypeAdvancedGet)
            m_currentWindow++;
    }
    else
    {
        int index = m_currentWindow;
        int masterIndex = -1;

        if(m_checkForMaster == MasterPolicyAuto)
        {
            while(index < m_windows->size())
            {
                if(m_windows->at(index).hook)
                {
                    masterIndex = index;
                    break;
                }

                index++;
            }
        }

        if(m_checkForMaster == MasterPolicyAuto && masterIndex >= 0)
        {
            m_currentWindow = masterIndex;
            qDebug("Found AUTO index %d", m_currentWindow);
        }
        else if(m_checkForMaster == MasterPolicySkip)
        {
            while(m_currentWindow < m_windows->size() && m_windows->at(m_currentWindow).hook)
                m_currentWindow++;
            qDebug("Found SKIP index %d", m_currentWindow);
        }
        else if(m_checkForMaster == MasterPolicyIgnore)
        {
            qDebug("Found IGNORE index %d", m_currentWindow);
        }
    }
}

void THT::loadNextWindow()
{
    nextLoadableWindowIndex(+1);

    if(m_currentWindow >= m_windows->size())
    {
        qDebug("Done for all windows");

        if(m_windows == &m_windowsDrop)
        {
            qDebug("Clearing drop list");
            m_windows->clear();
        }

        busy(false);
        activateRightWindowAtEnd();

        m_wasActive = 0;
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

void THT::loadTicker(const QString &ticker, MasterLoadingPolicy masterPolicy)
{
    qDebug("Load ticker \"%s\"", qPrintable(ticker));

    if(m_locked)
    {
        qDebug("Locked");
        return;
    }

    if(m_running)
    {
        qDebug("In progress, won't load a new ticker");
        return;
    }

    if(m_sectors && sender() != m_sectors)
        m_sectors->showTicker(ticker);

    checkWindows();

    if(m_windows->isEmpty())
    {
        qDebug("No windows configured");
        return;
    }

    m_ticker = ticker;
    m_currentWindow = 0;
    m_checkForMaster = masterPolicy;

    if(!m_wasActive)
        m_wasActive = winId();

    nextLoadableWindowIndex();

    if(m_currentWindow >= m_windows->size())
    {
        qDebug("Cannot find where to load the ticker");
        activateRightWindowAtEnd();
        m_wasActive = 0;
        return;
    }

    m_running = true;
    m_startupTime = QDateTime::currentMSecsSinceEpoch();

    busy(true);

    m_lastActiveWindow = qApp->activeWindow();

    m_timerLoadToNextWindow->start();
}

void THT::startDelayedScreenshot(bool allowKbd)
{
    qDebug("Taking screenshot");

    m_takeScreen->setDisabled();

    m_useKeyboardInRegion = allowKbd;
    m_wasVisible = isVisible();

    hide();

    QTimer::singleShot(m_wasVisible ? 50 : 0, this, SLOT(slotTakeScreenshotReal()));
}

void THT::activate()
{
    qDebug("Activating");

    if(m_sectors)
        raiseWindow(m_sectors);

    raiseWindow(this);

    if(m_sectors && m_lastActiveWindow == m_sectors)
        m_sectors->activateWindow();
    else
        activateWindow();

    m_lastActiveWindow = 0;
}

void THT::slotAdjustSize()
{
    qDebug("Resizing automatically");

    layout()->activate();
    resize(minimumSizeHint().width(), height());
}

void THT::slotCheckActive()
{
    if(m_windows->isEmpty())
    {
        qDebug("Window list is empty");
        m_running = false;
        return;
    }

    Link &link = (*m_windows)[m_currentWindow];

    if(GetForegroundWindow() == link.hwnd)
    {
        qDebug("Found window, sending data");

        QString add;
        bool okToLoad = false;

        if(link.subControl)
        {
            if(setForeignFocus(link))
                okToLoad = true;
        }
        else
            okToLoad = true;

        // real ticker to load
        QString ticker;

        // determine the real ticker from the private ticker
        if(m_ticker.startsWith(THT_PRIVATE_TICKER_PREFIX))
        {
            ticker = m_ticker.right(m_ticker.length() - qstrlen(THT_PRIVATE_TICKER_PREFIX));

            // find mapping [ticker] => [predefined ticker data]
            PredefinedTickers::const_iterator it = m_predefined.find(ticker);

            if(it != m_predefined.end())
            {
                // find mapping [link type] => [ticker to load]
                PredefinedTickerMappings::const_iterator itm = it.value().mappings.find(link.type);

                // real ticker
                if(itm != it.value().mappings.end())
                {
                    if(!itm.value().isEmpty())
                        ticker = itm.value();
                }

                okToLoad = true;
            }
            else
                qWarning("Private ticker \"%s\" doesn't have a mapping", qPrintable(m_ticker));
        }
        else
        {
            ticker = m_ticker;
            okToLoad = true;
        }

        // load ticker
        if(okToLoad)
        {
            if(link.type == LinkTypeAdvancedGet
                && ui->checkNyse->isChecked()
                && !m_ticker.startsWith(QChar('$')))
            add = "=N";

            sendString(ticker + add, link.type);
        }

        loadNextWindow();
    }
    else
    {
        if(QDateTime::currentMSecsSinceEpoch() - m_startupTime > THT_WINDOW_STARTUP_TIMEOUT)
        {
            qDebug("Failed to wait the window due to timeout");
            loadNextWindow();
        }
        else
        {
            qDebug("Cannot find window, continuing to search");
            m_timerLoadToNextWindow->start();
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

void THT::slotPlugins()
{
    PluginManager mgr(this);
    mgr.exec();
}

void THT::slotQuit()
{
    qApp->quit();
}

void THT::slotOptions()
{
    bool oldDups = SETTINGS_GET_BOOL(SETTING_ALLOW_DUPLICATES);
    bool oldMini = SETTINGS_GET_BOOL(SETTING_MINI_TICKER_ENTRY);
    bool oldRestoreLP = SETTINGS_GET_BOOL(SETTING_RESTORE_LINKS_AT_STARTUP);

    Options opt(this);

    if(opt.exec() == QDialog::Accepted)
    {
        opt.saveSettings();
        rebuildUi();

        // always on top?
        QList<QWidget *> widgets = QList<QWidget *>() << this << m_sectors;

        foreach(QWidget *w, widgets)
        {
            if(!w)
                continue;

            Qt::WindowFlags flags = w->windowFlags();

            if(SETTINGS_GET_BOOL(SETTING_ONTOP))
                flags |= Qt::WindowStaysOnTopHint;
            else
                flags &= ~Qt::WindowStaysOnTopHint;

            w->setWindowFlags(flags);
            w->show();
        }

        m_tray->setVisible(SETTINGS_GET_BOOL(SETTING_HIDE_TO_TRAY));

        // delete duplicates
        if(oldDups && !SETTINGS_GET_BOOL(SETTING_ALLOW_DUPLICATES))
        {
            foreach(List *l, m_lists)
                l->removeDuplicates();
        }

        // reconfigure mini ticker entry
        if(oldMini != SETTINGS_GET_BOOL(SETTING_MINI_TICKER_ENTRY))
        {
            foreach(List *l, m_lists)
                l->reconfigureMiniTickerEntry();
        }

        if(!oldRestoreLP && SETTINGS_GET_BOOL(SETTING_RESTORE_LINKS_AT_STARTUP))
            m_linksChanged = true;

        // reset geometry
        if(!SETTINGS_GET_BOOL(SETTING_SAVE_GEOMETRY))
        {
            SETTINGS_SET_SIZE(SETTING_SIZE, QSize(), Settings::NoSync);
            SETTINGS_SET_POINT(SETTING_POSITION, QPoint(), Settings::NoSync);

            SETTINGS_SET_SIZE(SETTING_NEIGHBORS_SIZE, QSize(), Settings::NoSync);
            SETTINGS_SET_POINT(SETTING_NEIGHBORS_POSITION, Tools::invalidQPoint); // also sync
        }

        reconfigureGlobalShortcuts();
    }
}

void THT::slotCopyLeft(const Ticker &ticker)
{
    qDebug("Copy ticker \"%s\" left", qPrintable(ticker.ticker));

    int index = m_lists.indexOf(qobject_cast<List *>(sender()));

    if(index < 0)
    {
        qWarning("Cannot find where to copy the ticker");
        return;
    }

    if(m_lists.size() < 2)
    {
        qWarning("Cannot copy from the single list");
        return;
    }

    if(!index)
        index = m_lists.size() - 1;
    else
        --index;

    m_lists[index]->addTicker(ticker);
}

void THT::slotCopyRight(const Ticker &ticker)
{
    qDebug("Copy ticker \"%s\"/%d right", qPrintable(ticker.ticker), ticker.priority);

    int index = m_lists.indexOf(qobject_cast<List *>(sender()));

    if(index < 0)
    {
        qWarning("Cannot find where to copy the ticker");
        return;
    }

    if(m_lists.size() < 2)
    {
        qWarning("Cannot copy from the single list");
        return;
    }

    if(index == m_lists.size()-1)
        index = 0;
    else
        ++index;

    m_lists[index]->addTicker(ticker);
}

void THT::slotCopyTo(const Ticker &ticker, int index)
{
    qDebug("Copy ticker \"%s\"/%d to list #%d", qPrintable(ticker.ticker), ticker.priority, index);

    if(index < 0 || index >= m_lists.size())
    {
        qWarning("Cannot find where to copy the ticker");
        return;
    }

    m_lists[index]->addTicker(ticker);
}

void THT::slotLoadTicker(const QString &ticker)
{
    m_windows = &m_windowsLoad;
    loadTicker(ticker);
}

void THT::slotLoadTicker()
{
    if(SETTINGS_GET_BOOL(SETTING_MINI_TICKER_ENTRY))
    {
        // find where to set the focus
        const QWidget *focused = focusWidget();
        List *found = 0;

        foreach(List *l, m_lists)
        {
            if(l->isAncestorOf(focused))
            {
                found = l;
                break;
            }
        }

        if(!found && m_lists.size())
            found = m_lists.at(0);

        found->focusMiniTickerEntry();
    }
    else
    {
        TickerInput ti(this);

        if(ti.exec() != QDialog::Accepted)
            return;

        slotLoadTicker(ti.ticker());
    }
}

void THT::slotLoadToNextWindow()
{
    HWND window = m_windows->at(m_currentWindow).hwnd;

    qDebug("Trying window %p", window);

    bringToFront(window);

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

void THT::slotTakeScreenshotFromGlobal()
{
    startDelayedScreenshot(qApp->activeWindow());
}

void THT::slotTakeScreenshot()
{
    startDelayedScreenshot(true);
}

void THT::slotTakeScreenshotReal()
{
    RegionSelect selector(m_useKeyboardInRegion
                          ? RegionSelect::UseKeyboard
                          : RegionSelect::DontUseKeyboard);

    int code = selector.exec();

    if(m_wasVisible)
        activate();

    // ignore screenshot
    if(code != QDialog::Accepted)
    {
        m_takeScreen->setEnabled();
        return;
    }

    QPixmap px = selector.selection();

    if(px.isNull())
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot take screenshot"));
        m_takeScreen->setEnabled();
        return;
    }

    // save screenshot
    SaveScreenshot s(px, this);

    s.exec();

    m_takeScreen->setEnabled();
}

void THT::slotClearLists()
{
    foreach(List *l, m_lists)
        l->clear();
}

void THT::slotClearLinks()
{
    qDebug("Clear links");

    m_windows = &m_windowsLoad;

    if(m_windows->isEmpty())
        return;

    MessageBeep(MB_OK);

    unhookEverybody();

    m_windows->clear();

    checkWindows();

    m_linksChanged = true;
}

void THT::slotManageLinks()
{
    LinkPointSession session;

    session.name = tr("New points");

    foreach(Link l, m_windowsLoad)
    {
        session.windows.append(LinkedWindow(l.hook, l.dropPoint));
    }

    LinkPointManager mgr(session, this);

    if(mgr.exec() == QDialog::Accepted && mgr.changed())
    {
        SETTINGS_SET_LINKS(SETTING_LINKS, mgr.links());
        rebuildLinks();
    }
}

void THT::slotLoadLinks()
{
    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return;

    QList<LinkedWindow> links = a->data().value<QList<LinkedWindow> >();

    slotClearLinks();

    foreach(LinkedWindow lw, links)
    {
        targetDropped(lw.point, lw.master ? MasterYes : MasterNo);
    }
}

void THT::slotLockLinks()
{
    m_locked = !m_locked;

    ui->stackBusy->setCurrentIndex(m_locked);

    static QWidgetList labels = QWidgetList()
                                << ui->labelLinks
                                << ui->labelLinks_n;

    QColor color = m_locked ? Qt::red : palette().color(QPalette::WindowText);
    QPalette pal;

    foreach(QWidget *l, labels)
    {
        pal = l->palette();
        pal.setColor(QPalette::WindowText, color);
        l->setPalette(pal);
    }
}

void THT::slotTickerDropped(const Ticker &ticker, const QPoint &p)
{
    m_windows = &m_windowsDrop;
    m_windows->clear();

    removeWindowMarker();

    Link link = checkTargetWindow(p, true);

    if(!link.hwnd)
        return;

    // our window
    if(link.hwnd == reinterpret_cast<HWND>(winId()))
    {
        int index = 0;

        foreach(List *l, m_lists)
        {
            if(l->contains(p))
            {
                qDebug("Dropped onto a list");
                slotCopyTo(ticker, index);
                break;
            }

            index++;
        }

        return;
    }

    checkWindow(&link);

    if(link.type == LinkTypeNotInitialized)
        return;

    m_windows->append(link);

    loadTicker(ticker.ticker, MasterPolicyIgnore);
}

void THT::drawWindowMarker()
{
    if(!IsWindow(m_drawnWindow))
        return;

    static HPEN pen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));

    HDC dc;
    HGDIOBJ oldPen, oldBrush;
    RECT rect;

    GetWindowRect(m_drawnWindow, &rect);

    dc = GetWindowDC(m_drawnWindow);

    if(!dc)
        return;

    oldPen = SelectObject(dc, pen);
    oldBrush = SelectObject(dc, GetStockObject(HOLLOW_BRUSH));

    Rectangle(dc, 0, 0, rect.right - rect.left, rect.bottom - rect.top);

    SelectObject(dc, oldPen);
    SelectObject(dc, oldBrush);

    ReleaseDC(m_drawnWindow, dc);
}

void THT::removeWindowMarker()
{
    if(!IsWindow(m_drawnWindow))
        return;

    InvalidateRect(m_drawnWindow, 0, TRUE);
    UpdateWindow(m_drawnWindow);
    RedrawWindow(m_drawnWindow, 0, 0, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

    m_drawnWindow = 0;
}

void THT::reconfigureGlobalShortcuts()
{
    m_takeScreen->setEnabled(SETTINGS_GET_BOOL(SETTING_GLOBAL_HOTKEY_SCREENSHOT));
    m_restore->setEnabled(SETTINGS_GET_BOOL(SETTING_GLOBAL_HOTKEY_RESTORE));
}

void THT::unhookEverybody()
{
    foreach(Link l, m_windowsLoad)
    {
        if(l.hook)
            l.unhook();
    }
}

void THT::bringToFront(HWND window)
{
    // window flags to set
    int flags = SW_SHOWNORMAL;

    if(IsZoomed(window))
        flags |= SW_SHOWMAXIMIZED;

    // try to switch to this window
    ShowWindow(window, flags);
    SetForegroundWindow(window);
}

void THT::masterHasBeenChanged(HWND hwnd, const QString &ticker)
{
    qDebug("Master has been changed in window %p with ticker \"%s\"", hwnd, qPrintable(ticker));

    // check the window id
    foreach(Link l, m_windowsLoad)
    {
        if(l.hook && l.hwnd != hwnd)
        {
            qDebug("Master has a different window id: existing(%p), changed(%p)", l.hwnd, hwnd);
            return;
        }
    }

    // attach to master and set focus to us
    HWND foregroundWindow = GetForegroundWindow();

    if(foregroundWindow != winId())
    {
        if(!m_wasActive)
            m_wasActive = foregroundWindow;

        const DWORD currentThreadId = GetCurrentThreadId();
        DWORD threadId = GetWindowThreadProcessId(foregroundWindow, 0);

        if(!AttachThreadInput(threadId, currentThreadId, TRUE))
        {
            qWarning("Cannot attach to the thread %ld (%ld)", threadId, GetLastError());
            return;
        }

        activate();
        SetForegroundWindow(winId());

        AttachThreadInput(threadId, currentThreadId, FALSE);
    }
    else
    {
        m_wasActive = 0;
        activate();
    }

    loadTicker(ticker, MasterPolicySkip);
}

void THT::activateRightWindowAtEnd()
{
    qDebug("Activating %p (this %p)", m_wasActive, winId());

    if(m_wasActive && m_wasActive != winId())
        bringToFront(m_wasActive);
    else
        activate();
}

void THT::slotFomcCheck()
{
    // start check timer again
    m_timerFomcCheck->start();

    // determine the New York time
    QDateTime datetime = m_newYorkDate->dateTime();

    if(datetime.isValid())
    {
        qDebug("New York time: %s", qPrintable(datetime.toString("dd.MM.yyyy hh:mm:ss")));

        // query FOMC date
        QString date = datetime.toString("yyyy MM dd");
        QList<QVariantList> lists = SqlTools::query("SELECT date FROM fomc WHERE date = :date", ":date", date);

        if(!lists.isEmpty() && lists.at(0).size() == 1)
        {
            qDebug("News from FOMC is today");
            ui->labelFomc->show();
            return;
        }
        else
            qDebug("Cannot query FOMC date");
    }
    else
        qDebug("New York time is invalid");

    ui->labelFomc->hide();
}

void THT::slotRestoreLinks()
{
    // restore link points
    if(SETTINGS_GET_BOOL(SETTING_RESTORE_LINKS_AT_STARTUP))
    {
        QList<LinkedWindow> list = SETTINGS_GET_LINKED_WINDOWS(SETTING_LAST_LINKS);

        foreach(LinkedWindow lw, list)
        {
            qDebug("Restoring link point: master(%s), %dx%d", lw.master ? "yes" : "no", lw.point.x(), lw.point.y());
            targetDropped(lw.point, lw.master ? MasterYes : MasterNo, false);
        }

        m_linksChanged = false;
    }
}

void THT::targetDropped(const QPoint &p, MasterSettings master, bool beep)
{
    m_windows = &m_windowsLoad;

    removeWindowMarker();

    Link link = checkTargetWindow(p, false);

    if(!link.hwnd)
        return;

    checkWindow(&link);

    if(link.type == LinkTypeNotInitialized)
        return;

    link.dropPoint = p;

    bool ismaster = false;

    switch(master)
    {
        case MasterNo:
            ismaster = false;
        break;

        case MasterYes:
            ismaster = true;
        break;

        case MasterAuto:
            ismaster = ui->target->mayBeMaster();
        break;
    }

    if(ismaster)
    {
        unhookEverybody();

        qDebug("Setting hook to process %ld", link.processId);

        link.hook = SetWinEventHook(EVENT_OBJECT_NAMECHANGE,
                                    EVENT_OBJECT_NAMECHANGE,
                                    0,
                                    WinEventProcCallback,
                                    link.processId,
                                    0,
                                    WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS | WINEVENT_SKIPOWNTHREAD);

        if(!link.hook)
            qWarning("Can't install hook to process %ld", link.processId);
    }

    // beep
    if(beep)
        MessageBeep(MB_OK);

    m_windows->append(link);

    checkWindows();

    m_linksChanged = true;
}

void THT::slotTargetMoving(const QPoint &pt)
{
    POINT pnt = {0};

    pnt.x = pt.x();
    pnt.y = pt.y();

    HWND rnewHwnd = RealChildWindowFromPoint(GetDesktopWindow(), pnt);
    HWND newHwnd = Tools::RealWindowFromPoint(pnt);

    if(m_drawnWindow != newHwnd)
        removeWindowMarker();

    if(!IsWindow(newHwnd) || rnewHwnd == reinterpret_cast<HWND>(winId()) || Tools::isDesktop(rnewHwnd))
        return;

    m_drawnWindow = newHwnd;

    drawWindowMarker();
}

void THT::slotTargetCancelled()
{
    removeWindowMarker();
}

// IPC message
void THT::slotMessageReceived(const QString &msg)
{
    qDebug("Got message \"%s\"", qPrintable(msg));

    if(msg == "wake up")
        activate();
}

void THT::slotLoadPredefinedTicker()
{
    QShortcut *s = qobject_cast<QShortcut *>(sender());

    if(!s)
        return;

    QString ticker = s->property("predefined-ticker-key").toString();

    if(ticker.isEmpty())
        return;

    slotLoadTicker(THT_PRIVATE_TICKER_PREFIX + ticker);
}

void THT::slotOpenOrCloseSearchTicker()
{
    bool done = false;

    foreach(List *l, m_lists)
    {
        if(l->searching())
        {
            l->stopSearching();
            done = true;
        }
    }

    if(done)
        return;

    foreach(List *l, m_lists)
    {
        if(l->hasFocus())
        {
            l->startSearching();
            break;
        }
    }
}

void THT::slotShowNeighbors(const QString &ticker)
{
    if(m_sectors)
    {
        m_sectors->showTicker(ticker);
        raiseWindow(m_sectors);
        m_sectors->activateWindow();
        return;
    }

    m_sectors = new TickerNeighbors(ticker, this);

    if(SETTINGS_GET_BOOL(SETTING_ONTOP))
        m_sectors->setWindowFlags(m_sectors->windowFlags() | Qt::WindowStaysOnTopHint);

    connect(m_sectors, SIGNAL(loadTicker(QString)), this, SLOT(slotLoadTicker(QString)));

    m_sectors->show();
}

void THT::slotFoolsDay()
{
    QDate day = QDate::currentDate();

    if(day.day() == 1 && day.month() == 4)
    {
        QMessageBox::information(this,
                                 QString(),
                                 QString("<p>%1</p><p align=right><i>%2</i></p>")
                                        .arg(tr("All large contemporary fortunes were acquired<br>in the most dishonorable way."))
                                        .arg(tr("\"The Little Golden Calf\" I.Ilf, E.Petrov")));

        SETTINGS_SET_BOOL(SETTING_FOOLSDAY_SEEN, true);
    }
}

void THT::slotFomcClicked()
{
    QDesktopServices::openUrl(QUrl("http://www.bloomberg.com/markets/economic-calendar"));
}

bool THT::setForeignFocus(const Link &link)
{
    const DWORD currentThreadId = GetCurrentThreadId();

    if(!AttachThreadInput(link.threadId, currentThreadId, TRUE))
    {
        qWarning("Cannot attach to the thread %ld (%ld)", link.threadId, GetLastError());
        return false;
    }

    HWND hwnd = SetFocus(link.subControl);

    if(link.subControlSupportsClearing)
        SendMessage(link.subControl, WM_SETTEXT, 0, 0);

    SendMessage(link.hwnd, WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_SYSCHARSET, 0x04090409);

    AttachThreadInput(link.threadId, currentThreadId, FALSE);

    if(!hwnd)
    {
        qWarning("Cannot set focus to the window %p (%ld)", link.subControl, GetLastError());
        return false;
    }

    return true;
}

void THT::rebuildLinks()
{
    qDebug("Rebuild link menu");

    QList<LinkPointSession> links = SETTINGS_GET_LINKS(SETTING_LINKS);

    QMenu *menu = ui->pushLinks->menu();

    menu->clear();

    foreach(LinkPointSession lp, links)
    {
        QAction *a = menu->addAction(lp.name, this, SLOT(slotLoadLinks()));
        a->setData(QVariant::fromValue(lp.windows));
    }

    if(!links.isEmpty())
        menu->addSeparator();

    menu->addAction(QIcon(":/images/links-customize.png"), tr("Customize..."), this, SLOT(slotManageLinks()));
}

void THT::raiseWindow(QWidget *w)
{
    if(!w)
        return;

    w->show();
    w->setWindowState(w->windowState() & ~Qt::WindowMinimized);
    w->raise();
}

void THT::Link::unhook()
{
    if(!hook)
        return;

    UnhookWinEvent(hook);
    hook = 0;
}
