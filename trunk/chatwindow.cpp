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
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QMovie>
#include <QDebug>
#include <QMenu>
#include <QIcon>

#include "QXmppVersionManager.h"
#include "QXmppConfiguration.h"
#include "QXmppMucManager.h"
#include "QXmppUtils.h"

#include "chatsettings.h"
#include "chatoptions.h"
#include "chatwindow.h"
#include "chatpage.h"
#include "settings.h"
#include "roominfo.h"
#include "tools.h"
#include "ui_chatwindow.h"

ChatWindow::ChatWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow)
{
    // pythonr@conference.jabber.ru
    ui->setupUi(this);

    m_unreadMessage = QIcon(":/images/unread.png");

    // context menu
    m_menu = new QMenu(this);

    m_menu->addAction(QIcon(":/images/options.png"), tr("Options..."), this, SLOT(slotOptions()));

    // XMPP client
    m_xmppClient = new QXmppClient(this);

    m_xmppClient->versionManager().setClientName("THT Chat");
    m_xmppClient->versionManager().setClientVersion(NVER_STRING);

    // MUC
    m_muc = new QXmppMucManager;
    m_xmppClient->addExtension(m_muc);

    connect(m_xmppClient, SIGNAL(error(QXmppClient::Error)), this, SLOT(slotError(QXmppClient::Error)));
    connect(m_xmppClient, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(m_xmppClient, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));

    ui->checkRememberPassword->setChecked(SETTINGS_GET_BOOL(SETTING_CHAT_REMEMBER_PASSWORD));

    if(ui->checkRememberPassword->isChecked())
        ui->linePassword->setText(SETTINGS_GET_STRING(SETTING_CHAT_PASSWORD));

    ui->lineJid->setText(SETTINGS_GET_STRING(SETTING_CHAT_JID));

    // corner widget
    QPushButton *b = new QPushButton("+", ui->tabs);
    connect(b, SIGNAL(clicked()), this, SLOT(slotAddTab()));
    ui->tabs->setCornerWidget(b);
    b->show();

    showSignInPage();

    if(SETTINGS_GET_BOOL(SETTING_CHAT_AUTO_LOGIN))
        ui->pushSignIn->click();

    // restore geometry
    if(SETTINGS_GET_BOOL(SETTING_SAVE_GEOMETRY))
    {
        QSize sz = SETTINGS_GET_SIZE(SETTING_CHAT_SIZE);

        if(sz.isValid())
            resize(sz);

        Tools::moveWindow(this, SETTINGS_GET_POINT(SETTING_CHAT_POSITION));
    }
}

ChatWindow::~ChatWindow()
{
    if(SETTINGS_GET_BOOL(SETTING_SAVE_GEOMETRY))
    {
        SETTINGS_SET_SIZE(SETTING_CHAT_SIZE, QSize(width(), height()), Settings::NoSync);
        SETTINGS_SET_POINT(SETTING_CHAT_POSITION, pos());
    }

    saveRooms();

    delete ui;
}

void ChatWindow::contextMenuEvent(QContextMenuEvent *e)
{
    e->accept();

    m_menu->exec(e->globalPos());
}

void ChatWindow::showSignInPage()
{
    ui->pushCancel->setEnabled(false);
    ui->pushSignIn->setEnabled(true);
    ui->lineJid->setEnabled(true);
    ui->linePassword->setEnabled(true);

    ui->stack->setCurrentIndex(0);
}

void ChatWindow::showChatsPage()
{
    ui->stack->setCurrentIndex(1);
}

void ChatWindow::showLoginStatus(const QString &s)
{
    ui->labelStatus->setText(s);
}

void ChatWindow::setTabName(QWidget *tab, const QString &roomName)
{
    int index = ui->tabs->indexOf(tab);

    if(index < 0)
    {
        qWarning("Unknown room has joined");
        return;
    }

    ui->tabs->setTabText(index, roomName.isEmpty() ? tr("Room") : roomName);
}

ChatPage *ChatWindow::createPage(bool checkForAutoLogin, const QString &jid, const QString &password)
{
    ChatPage *p = new ChatPage(m_muc, checkForAutoLogin, jid, password, ui->tabs);

    connect(p, SIGNAL(requestJoin(QString)), this, SLOT(slotJoinRequested(QString)));
    connect(p, SIGNAL(joined(QString)),      this, SLOT(slotJoined(QString)));
    connect(p, SIGNAL(left()),               this, SLOT(slotLeft()));
    connect(p, SIGNAL(nameChanged(QString)), this, SLOT(slotNameChanged(QString)));
    connect(p, SIGNAL(message()),            this, SLOT(slotMessage()));
    connect(p, SIGNAL(openTicker(QString)),  this, SIGNAL(openTicker(QString)));

    return p;
}

void ChatWindow::saveRooms()
{
    if(chatsPage() && SETTINGS_GET_BOOL(SETTING_CHAT_SAVE_ROOMS))
    {
        QList<RoomInfo> rooms;
        int index = 0;
        ChatPage *p;

        while((p = qobject_cast<ChatPage *>(ui->tabs->widget(index++))))
            rooms.append(RoomInfo(p->jid(), p->password()));

        qDebug("Saving rooms: %d", rooms.size());

        SETTINGS_SET_ROOMS_INFO(SETTING_CHAT_ROOMS_INFO, rooms);
    }
}

void ChatWindow::restoreRooms()
{
    // add previous rooms
    if(SETTINGS_GET_BOOL(SETTING_CHAT_SAVE_ROOMS))
    {
        QList<RoomInfo> rooms = SETTINGS_GET_ROOMS_INFO(SETTING_CHAT_ROOMS_INFO);

        qDebug("Restoring rooms: %d", rooms.size());

        foreach(RoomInfo ri, rooms)
        {
            ui->tabs->addTab(createPage(true, ri.jid, ri.password), tr("Room"));
        }
    }
}

void ChatWindow::removeTabs()
{
    QWidget *p;
    int index = 0;

    while((p = ui->tabs->widget(index++)))
        delete p;

    ui->tabs->clear();
}

bool ChatWindow::chatsPage()
{
    return ui->stack->currentIndex();
}

void ChatWindow::slotSignIn()
{
    m_xmppClient->disconnectFromServer();

    SETTINGS_SET_STRING(SETTING_CHAT_JID, ui->lineJid->text());
    SETTINGS_SET_BOOL(SETTING_CHAT_REMEMBER_PASSWORD, ui->checkRememberPassword->isChecked());

    ui->pushCancel->setEnabled(true);
    ui->pushSignIn->setEnabled(false);
    ui->lineJid->setEnabled(false);
    ui->linePassword->setEnabled(false);

    m_xmppClient->configuration().setJid(ui->lineJid->text());
    m_xmppClient->configuration().setPassword(ui->linePassword->text());

    showLoginStatus(tr("Connecting..."));

    m_xmppClient->connectToServer(m_xmppClient->configuration());
}

void ChatWindow::slotCancelSignIn()
{
    m_xmppClient->disconnectFromServer();
    showSignInPage();
    showLoginStatus(tr("Sign in cancelled"));
}

void ChatWindow::slotError(QXmppClient::Error error)
{
    if(chatsPage())
    {
        saveRooms();
        removeTabs();
    }

    showSignInPage();

    QString err;

    switch(error)
    {
        case QXmppClient::SocketError:
            err = tr("Socket error");
        break;

        case QXmppClient::KeepAliveError:
            err = tr("Keep alive error");
        break;

        case QXmppClient::XmppStreamError:
            switch(m_xmppClient->xmppStreamError())
            {
                case QXmppStanza::Error::NotAuthorized:
                    err = tr("Invalid password");
                break;

                default:
                    err = tr("Stream error");
                break;
            }
        break;

        default:
            err = tr("Unknown error");
        break;
    }

    showLoginStatus("<font color=red><b>" + err + "</b></font>");
}

void ChatWindow::slotConnected()
{
    // remove old tabs
    removeTabs();

    if(ui->checkRememberPassword->isChecked())
        SETTINGS_SET_STRING(SETTING_CHAT_PASSWORD, ui->linePassword->text());
    else
        SETTINGS_SET_STRING(SETTING_CHAT_PASSWORD, QString());

    restoreRooms();

    // TODO new "+" button
    if(!ui->tabs->count())
        slotAddTab();

    showChatsPage();
}

void ChatWindow::slotDisconnected()
{
    if(chatsPage())
        showSignInPage();
}

void ChatWindow::slotAddTab()
{
    ui->tabs->setCurrentIndex(ui->tabs->addTab(createPage(false), tr("Room")));
}

void ChatWindow::slotTabCloseRequested(int index)
{
    ChatPage *p = qobject_cast<ChatPage *>(ui->tabs->widget(index));

    if(!p)
    {
        qWarning("Cannot determine the tab to remove");
        return;
    }

    qDebug("Removing page \"%s\"", qPrintable(p->roomName()));

    delete p;
}

void ChatWindow::slotJoined(const QString &roomName)
{
    setTabName(qobject_cast<QWidget *>(sender()), roomName);
}

void ChatWindow::slotLeft()
{
    setTabName(qobject_cast<QWidget *>(sender()), tr("Room"));
}

void ChatWindow::slotNameChanged(const QString &roomName)
{
    setTabName(qobject_cast<QWidget *>(sender()), roomName);
}

void ChatWindow::slotJoinRequested(const QString &jid)
{
    int index = 0;
    ChatPage *p;

    while((p = qobject_cast<ChatPage *>(ui->tabs->widget(index))))
    {
        if(p->jid() == jid && p != sender())
        {
            qDebug("Room \"%s\" is already joined, switching to it", qPrintable(jid));
            sender()->deleteLater();
            ui->tabs->setCurrentIndex(index);
            return;
        }

        index++;
    }

    p = qobject_cast<ChatPage *>(sender());

    if(p)
        p->proceedJoin();
}

void ChatWindow::slotCurrentTabChanged(int index)
{
    ui->tabs->setTabIcon(index, QIcon());
}

void ChatWindow::slotMessage()
{
    int from = ui->tabs->indexOf(qobject_cast<ChatPage *>(sender()));
    int active = ui->tabs->currentIndex();

    if(from < 0 || active < 0)
        return;

    if(from != active)
    {
        qDebug("Setting page %d to be in an alert state", from);
        ui->tabs->setTabIcon(from, m_unreadMessage);
    }

    QApplication::alert(this);
}

void ChatWindow::slotOptions()
{
    ChatOptions opt(this);

    if(opt.exec() == QDialog::Accepted)
    {
        opt.saveSettings();

        const int fontSize = SETTINGS_GET_INT(SETTING_CHAT_FONT_SIZE);
        int index = 0;
        ChatPage *p;

        while((p = qobject_cast<ChatPage *>(ui->tabs->widget(index++))))
            p->setFontSize(fontSize);
    }
}
