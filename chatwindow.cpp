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
#include <QShortcut>
#include <QTextEdit>
#include <QAction>
#include <QMovie>
#include <QDebug>
#include <QMenu>
#include <QIcon>
#include <QList>

#include "QXmppMessageReceiptManager.h"
#include "QXmppVersionManager.h"
#include "QXmppConfiguration.h"
#include "QXmppMucManager.h"
#include "QXmppPresence.h"
#include "QXmppMessage.h"
#include "QXmppUtils.h"

#include "chatsettings.h"
#include "chatoptions.h"
#include "chatwindow.h"
#include "chattools.h"
#include "chatpage.h"
#include "settings.h"
#include "roominfo.h"
#include "tools.h"
#include "ui_chatwindow.h"

ChatWindow::ChatWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);

    //: Means "Based on <some technology>". The technology will be added in runtime by the application
    ui->labelBasedOn->setText(tr("Based on") + " <a href=\"http://xmpp.org\">XMPP</a>");

    // XMPP client
    m_xmppClient = new QXmppClient(this);

    m_xmppClient->versionManager().setClientName("THT Chat");
    m_xmppClient->versionManager().setClientVersion(NVER_STRING);

    // listen for delivery signals
    m_receiptManager = new QXmppMessageReceiptManager;
    m_xmppClient->addExtension(m_receiptManager);

    // MUC
    m_muc = new QXmppMucManager;
    m_xmppClient->addExtension(m_muc);

    connect(m_xmppClient, SIGNAL(presenceReceived(QXmppPresence)), this, SLOT(slotPresenceReceived(QXmppPresence)));
    connect(m_xmppClient, SIGNAL(error(QXmppClient::Error)), this, SLOT(slotError(QXmppClient::Error)));
    connect(m_xmppClient, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(m_xmppClient, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    connect(m_xmppClient, SIGNAL(messageReceived(QXmppMessage)), this, SLOT(slotMessageReceived(QXmppMessage)));

    if(SETTINGS_GET_BOOL(SETTING_CHAT_AUTO_LOGIN))
        ui->linePassword->setText(SETTINGS_GET_STRING(SETTING_CHAT_PASSWORD));

    ui->lineJid->setText(SETTINGS_GET_STRING(SETTING_CHAT_JID));

    // "Close" hotkeys
    QList<QKeySequence> kss = QKeySequence::keyBindings(QKeySequence::Close);

    foreach(QKeySequence ks, kss)
    {
        new QShortcut(ks, this, SLOT(slotCloseCurrentTab()));
    }

    QShortcut *newTabShortcut = new QShortcut(QKeySequence::New, this, SLOT(slotAddTab()));
    QShortcut *helpShortcut = new QShortcut(QKeySequence::HelpContents, this, SLOT(slotHelp()));

    // context menu
    m_menu = new QMenu(this);

    //: This is the label on a menu item that user clicks to issue the command. Means "Add a new room"
    m_actionAddRoom = new QAction(QIcon(":/images/addroom.png"), tr("Add room") + '\t' + newTabShortcut->key().toString(), this);
    connect(m_actionAddRoom, SIGNAL(triggered()), this, SLOT(slotAddTab()));
    m_menu->addAction(m_actionAddRoom);

    //: This is the label on a menu item that user clicks to issue the command. Means "Disconnect from the server"
    m_actionDisconnect = new QAction(tr("Disconnect"), this);
    connect(m_actionDisconnect, SIGNAL(triggered()), m_xmppClient, SLOT(disconnectFromServer()));
    m_menu->addAction(m_actionDisconnect);

    m_menu->addSeparator();
    m_menu->addAction(QIcon(":/images/options.png"), tr("Options..."), this, SLOT(slotOptions()));
    m_menu->addSeparator();
    //: Noun
    m_menu->addAction(tr("Help") + "..."  + '\t' + helpShortcut->key().toString(), this, SLOT(slotHelp()));

    ui->pushAddTab->setText(tr("Add room"));
    ui->pushAddTab->setIcon(m_actionAddRoom->icon());

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

    m_actionAddRoom->setEnabled(m_xmppClient->isConnected());
    m_actionDisconnect->setEnabled(m_xmppClient->isConnected());

    m_menu->exec(e->globalPos());
}

void ChatWindow::showSignInPage()
{
    ui->pushCancel->setEnabled(false);
    ui->pushSignIn->setEnabled(true);
    ui->lineJid->setEnabled(true);
    ui->linePassword->setEnabled(true);

    ui->labelStatus->clear();
    ui->stack->setCurrentIndex(0);
}

void ChatWindow::showChatsPage()
{
    ui->stack->setCurrentIndex(1);
}

void ChatWindow::showAddTabPage()
{
    qDebug("Showing Add Tab page");
    ui->stack->setCurrentIndex(2);
}

void ChatWindow::showLoginStatus(const QString &s)
{
    ui->labelStatus->setText(s);
}

void ChatWindow::showError(const QString &s)
{
    showLoginStatus("<font color=red><b>" + s + "</b></font>");
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

ChatPage *ChatWindow::createPage(bool checkForAutoLogin, const QString &jid, const QString &nick, const QString &password)
{
    qDebug("Creating page for jid \"%s\"", qPrintable(jid));

    ChatPage *p = new ChatPage(m_xmppClient, m_muc, checkForAutoLogin, jid, nick, password, ui->tabs);

    connect(p, SIGNAL(requestJoin(QString)), this, SLOT(slotJoinRequested(QString)));
    connect(p, SIGNAL(joined(QString)),      this, SLOT(slotJoined(QString)));
    connect(p, SIGNAL(left()),               this, SLOT(slotLeft()));
    connect(p, SIGNAL(nameChanged(QString)), this, SLOT(slotNameChanged(QString)));
    connect(p, SIGNAL(message()),            this, SLOT(slotMessage()));
    connect(p, SIGNAL(openTicker(QString)),  this, SIGNAL(openTicker(QString)));

    connect(m_receiptManager, SIGNAL(messageDelivered(QString,QString)), p, SLOT(slotMessageDelivered(QString,QString)));

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
            rooms.append(RoomInfo(p->jid(), p->nick(), p->password()));

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
            ui->tabs->addTab(createPage(true, ri.jid, ri.nick, ri.password), tr("Room"));
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

void ChatWindow::savePassword()
{
    if(SETTINGS_GET_BOOL(SETTING_CHAT_AUTO_LOGIN))
        SETTINGS_SET_STRING(SETTING_CHAT_PASSWORD, ui->linePassword->text());
    else
        SETTINGS_SET_STRING(SETTING_CHAT_PASSWORD, QString());
}

void ChatWindow::slotSignIn()
{
    m_xmppClient->disconnectFromServer();

    QString jid = ui->lineJid->text();

    if(jid.isEmpty())
    {
        qWarning("Jid is empty");
        showError(tr("JID is empty"));
        return;
    }

    SETTINGS_SET_STRING(SETTING_CHAT_JID, jid);

    ui->pushCancel->setEnabled(true);
    ui->pushSignIn->setEnabled(false);
    ui->lineJid->setEnabled(false);
    ui->linePassword->setEnabled(false);

    m_xmppClient->configuration().setJid(jid);
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

void ChatWindow::slotPresenceReceived(const QXmppPresence &presence)
{
    QString from = QXmppUtils::jidToBareJid(presence.from());

    int index = 0;
    ChatPage *p;

    while((p = qobject_cast<ChatPage *>(ui->tabs->widget(index++))))
    {
        if(p->jid() == from)
        {
            p->presenceChanged(presence);
            break;
        }
    }

    switch(presence.type())
    {
        case QXmppPresence::Subscribe:
        {
            QXmppPresence unsubscribed;
            unsubscribed.setTo(from);
            unsubscribed.setType(QXmppPresence::Unsubscribed);
            m_xmppClient->sendPacket(unsubscribed);
        }
        break;

        default:
        break;
    }
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
            err = tr("Network error");
        break;

        case QXmppClient::KeepAliveError:
            //: Also can be translated as "Error maintain the connection"
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

    showError(err);
}

void ChatWindow::slotConnected()
{
    // remove old tabs
    removeTabs();

    savePassword();
    restoreRooms();

    if(!ui->tabs->count())
        showAddTabPage();
    else
        showChatsPage();
}

void ChatWindow::slotDisconnected()
{
    if(chatsPage())
        showSignInPage();
}

void ChatWindow::slotMessageReceived(const QXmppMessage &msg)
{
    if(msg.type() != QXmppMessage::Normal)
        return;

    if(!msg.mucInvitationJid().isEmpty())
    {
        int index = 0;
        ChatPage *p;

        qDebug("Invitation to \"%s\"", qPrintable(msg.mucInvitationJid()));

        while((p = qobject_cast<ChatPage *>(ui->tabs->widget(index++))))
        {
            if(p->jid() == msg.mucInvitationJid())
            {
                if(p->isJoined())
                    return;
                else
                    break;
            }
        }

        if(QMessageBox::question(this,
                                 //: Noun
                                 tr("Invitation"),
                                 //: Message displayed to the user
                                 tr("You have been invited to room %1%2%3%4<br><br>Open the room now?")
                                    .arg("<b>" + msg.mucInvitationJid() + "</b>")
                                    .arg(msg.mucInvitationPassword().isEmpty() ? QString() : ("<br>" + tr("Password:") + ' ' +  msg.mucInvitationPassword()))
                                    .arg(msg.mucInvitationReason().isEmpty() ? QString() : ("<br>" + tr("Reason:") + ' ' +  msg.mucInvitationReason()))
                                    .arg(msg.body().isEmpty() ? QString() : ("<br>" + tr("Message:") + ' ' + ChatTools::escapeBrackets(msg.body()))),
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            if(p)
                ui->tabs->setCurrentWidget(p);
            else
                ui->tabs->setCurrentIndex(ui->tabs->addTab(createPage(true, msg.mucInvitationJid(), QString(), msg.mucInvitationPassword()), tr("Room")));
        }
    }
}

void ChatWindow::slotAddTab()
{
    showChatsPage();
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

    QString key = p->jid() + THT_CHAT_SPLITTER_STATE_SUFFIX;

    qDebug("Removing page \"%s\"", p->roomName().toUtf8().constData());

    delete p;

    Settings::instance()->remove(key);

    saveRooms();

    if(!ui->tabs->count())
        showAddTabPage();
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
    {
        saveRooms();
        p->proceedJoin();
    }
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
        ui->tabs->setTabIcon(from, ChatTools::unreadIcon());
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

        savePassword();
        saveRooms();
    }
}

void ChatWindow::slotHelp()
{
    QMessageBox::information(this,
                             tr("Help"),
                             QString("<table cellspacing=6>"
                                     "<tr><td align=center>=ABC=</td><td>%1</td></tr>"
                                     "<tr><td align=center>/ABC</td><td>%2</td></tr>"
                                     "<tr><td align=center>//Gold</td><td>%3</td></tr>"
                                     "<tr><td align=center>//Gold=A</td><td>%4</td></tr>"
                                     "<tr><td align=center>//Gold=D</td><td>%5</td></tr>"
                                     "<tr><td align=center>//Gold=N</td><td>%6</td></tr>"
                                     "<tr><td align=center>//Gold=ND</td><td>%7</td></tr>"
                                     "</table>")
                             //: Don't translate "ABC" - this is a fixed ticker name
                             .arg(tr("Send the ticker \"ABC\" to the chat. User can click on it and it will be loaded into all the linked windows"))
                             //: Don't translate "ABC" - this is a fixed ticker name
                             .arg(tr("Show the ticker information about the ticker \"ABC\""))
                             //: Don't translate "Gold" - this is a fixed industry name. "Exchange" is a market exchange
                             .arg(tr("Show the tickers from the industry \"Gold\" (all exchanges, tickers are sorted by capitalization)"))
                             //: Don't translate "Gold" - this is a fixed industry name
                             .arg(tr("Show the AMEX tickers from the industry \"Gold\""))
                             //: Don't translate "Gold" - this is a fixed industry name
                             .arg(tr("Show the NASDAQ tickers from the industry \"Gold\""))
                             //: Don't translate "Gold" - this is a fixed industry name
                             .arg(tr("Show the NYSE tickers from the industry \"Gold\""))
                             //: Don't translate "Gold" - this is a fixed industry name
                             .arg(tr("Show the NYSE and NASDAQ tickers from the industry \"Gold\" (you can mix exchanges)"))
                             );
}

void ChatWindow::slotCloseCurrentTab()
{
    if(chatsPage())
        slotTabCloseRequested(ui->tabs->currentIndex());
}
