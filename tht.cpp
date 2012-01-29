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
#include <QMessageBox>
#include <QTimer>
#include <QIcon>
#include <QMenu>

#include <windows.h>

#include "options.h"
#include "about.h"
#include "list.h"
#include "tht.h"
#include "ui_tht.h"

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

    // TODO
    m_windows.append((HWND)6489276);
    m_windows.append((HWND)853432);

    m_ticker = "ABC";

    new List("section1", this);
    //QTimer::singleShot(500, this, SLOT(slotTimer()));
}

THT::~THT()
{
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

void THT::slotTimer()
{
    if(m_windows.isEmpty())
    {
        m_running = false;
        return;
    }

    m_running = true;

    HWND window = m_windows.at(0);

    // window flags to set
    int flags = SW_SHOWNORMAL;

    if(IsZoomed(window))
        flags |= SW_SHOWMAXIMIZED;

    // try to switch to this window
    ShowWindow(m_windows.at(0), flags);
    SetForegroundWindow(m_windows.at(0));

    m_timerCheckActive->start(0);
}

void THT::slotCheckActive()
{
    if(m_windows.isEmpty())
    {
        qDebug("THT: Window list is empty");
        m_running = false;
        return;
    }

    HWND window = m_windows.at(0);
    WINDOWINFO pwi = {0};
    pwi.cbSize = sizeof(WINDOWINFO);
    GetWindowInfo(window, &pwi);

    if(GetForegroundWindow() == window && pwi.dwWindowStatus == WS_ACTIVECAPTION)
    {
        qDebug("THT: Found window");
        sendString(m_ticker);
        m_windows.pop_front();
        QTimer::singleShot(100, this, SLOT(slotTimer()));
    }
    else
    {
        qDebug("THT: Cannot find window, continuing to search");
        m_timerCheckActive->start();
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
    }
}
