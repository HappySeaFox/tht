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

#include <QContextMenuEvent>
#include <QGridLayout>
#include <QMessageBox>
#include <QDateTime>
#include <QSettings>
#include <QTimer>
#include <QIcon>
#include <QMenu>

#include <windows.h>

#include "options.h"
#include "about.h"
#include "list.h"
#include "tht.h"
#include "ui_tht.h"

static const int THT_WINDOW_STARTUP_TIMEOUT = 2000;

THT::THT(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::THT),
    m_running(false)
{
    QIcon icon;
    icon.addFile(":/images/chart.png");
    icon.addFile(":/images/chart_24.png");
    icon.addFile(":/images/chart_32.png");
    icon.addFile(":/images/chart_48.png");
    icon.addFile(":/images/chart_64.png");
    setWindowIcon(icon);

    ui->setupUi(this);

    m_menu = new QMenu(this);
    m_menu->addAction(tr("Options..."), this, SLOT(slotOptions()));
    m_menu->addSeparator();
    m_menu->addAction(tr("About THT"), this, SLOT(slotAbout()));
    m_menu->addAction(tr("About Qt"), this, SLOT(slotAboutQt()));

    m_timerCheckActive = new QTimer(this);
    m_timerCheckActive->setSingleShot(true);
    m_timerCheckActive->setInterval(50);
    connect(m_timerCheckActive, SIGNAL(timeout()), this, SLOT(slotCheckActive()));

    m_timerLoadToNextWindow = new QTimer(this);
    m_timerLoadToNextWindow->setSingleShot(true);
    m_timerLoadToNextWindow->setInterval(0);
    connect(m_timerLoadToNextWindow, SIGNAL(timeout()), this, SLOT(slotLoadToNextWindow()));

    // TODO
    m_windows.append((HWND)2621942);
    m_windows.append((HWND)4719922);

    // layout
    m_layout = new QGridLayout(this);
    setLayout(m_layout);

    rebuildUi();

    // restore geometry
    QSettings settings;

    settings.beginGroup("settings");

    if(settings.value("save-geometry", true).toBool())
    {
        QSize sz = settings.value("size").toSize();

        if(sz.isValid())
            resize(sz);

        QPoint pt = settings.value("position").toPoint();

        if(!pt.isNull())
            move(pt);
    }
}

THT::~THT()
{
    QSettings settings;

    settings.beginGroup("settings");

    if(settings.value("save-geometry", true).toBool())
    {
        settings.setValue("size", size());
        settings.setValue("position", pos());
    }

    delete ui;
}

void THT::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
    m_menu->exec(event->globalPos());
}

void THT::sendString(const QString &oticker) const
{
    if(oticker.isEmpty())
    {
        qDebug("THT: String is empty, nothing to send");
        return;
    }

    QString ticker = oticker + QChar(VK_RETURN);

    KEYBDINPUT kb = {0};
    const int numberOfSymbols = ticker.length()*2 + 2; // letters + return
    INPUT *input = new INPUT [numberOfSymbols];

    if(!input)
    {
        qDebug("THT: Cannot allocate memory");
        return;
    }

    for(int i = 0, j = 0;i < ticker.length();i++, j+=2)
    {
        memset(&input[j], 0, sizeof(INPUT));

        // assign virtual key
        kb.wVk = ticker.at(i).toAscii();
        kb.dwFlags = 0;
        input[j].type = INPUT_KEYBOARD;

        // generate down
        kb.dwFlags = KEYEVENTF_EXTENDEDKEY;

        input[j].ki = kb;

        // generate up
        kb.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_EXTENDEDKEY;

        input[j+1].ki = kb;
    }

    SendInput(numberOfSymbols, input, sizeof(INPUT));

    delete input;
}

void THT::rebuildUi()
{
    // check the number of lists
    QSettings settings;
    bool ok;

    settings.beginGroup("settings");
    int nlists = settings.value("number-of-lists", 3).toUInt(&ok);
    settings.endGroup();

    if(!ok)
        nlists = 3;

    if(nlists < 1 || nlists > 5)
        nlists = 3;

    // create more
    if(nlists > m_lists.size())
    {
        while(nlists > m_lists.size())
        {
            List *list = new List(QString("tickers-%1").arg(m_lists.size()+1), this);

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
            settings.beginGroup(QString("tickers-%1").arg(m_lists.size()));
            settings.remove(QString());
            settings.endGroup();

            delete m_lists.last();
            m_lists.removeLast();
        }
    }
}

void THT::checkWindows()
{
    RECT rect;

    QList<HWND>::iterator itEnd = m_windows.end();

    for(QList<HWND>::iterator it = m_windows.begin();it != itEnd;++it)
    {
        if(!GetWindowRect(*it, &rect))
        {
            qDebug("THT: Window id 0x%x is not valid, removing (%ld)", (uint)(*it), GetLastError());
            it = --m_windows.erase(it);
        }
    }
}

void THT::loadNextWindow()
{
    m_currentWindow++;

    if(m_currentWindow >= m_windows.size())
    {
        qDebug("THT: Done for all windows");
        activateWindow();
        m_running = false;
    }
    else
        m_timerLoadToNextWindow->start();
}

void THT::slotCheckActive()
{
    if(m_windows.isEmpty())
    {
        qDebug("THT: Window list is empty");
        m_running = false;
        return;
    }

    HWND window = m_windows.at(m_currentWindow);
    WINDOWINFO pwi = {0};
    pwi.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(window, &pwi);

    if(GetForegroundWindow() == window && pwi.dwWindowStatus == WS_ACTIVECAPTION)
    {
        qDebug("THT: Found window, sending data");
        sendString(m_ticker);
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

    m_timerLoadToNextWindow->start();
}

void THT::slotLoadToNextWindow()
{
    HWND window = m_windows.at(m_currentWindow);

    qDebug("THT: Trying window 0x%x", (uint)window);

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

