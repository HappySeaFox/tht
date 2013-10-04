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

#include <QDesktopServices>
#include <QListWidgetItem>
#include <QTextDocument>
#include <QInputDialog>
#include <QApplication>
#include <QTextBrowser>
#include <QMapIterator>
#include <QMessageBox>
#include <QListWidget>
#include <QClipboard>
#include <QScrollBar>
#include <QDateTime>
#include <QKeyEvent>
#include <QSplitter>
#include <QTabBar>
#include <QTimer>
#include <QDebug>
#include <QMenu>
#include <QMap>
#include <QUrl>
#include <Qt>

#include "QXmppConfiguration.h"
#include "QXmppMucManager.h"
#include "QXmppPresence.h"
#include "QXmppMessage.h"
#include "QXmppClient.h"
#include "QXmppUtils.h"
#include "QXmppMucIq.h"

#include "sendinvitations.h"
#include "coloranimation.h"
#include "configureroom.h"
#include "messagedialog.h"
#include "chatsettings.h"
#include "chatmessages.h"
#include "chattools.h"
#include "settings.h"
#include "chatpage.h"
#include "sqltools.h"
#include "tools.h"
#include "ui_chatpage.h"

ChatPage::ChatPage(QXmppClient *client,
                   QXmppMucManager *manager,
                   bool checkForAutoLogin,
                   const QString &jid,
                   const QString &nick,
                   const QString &password,
                   QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatPage),
    m_xmppClient(client),
    m_muc(manager),
    m_room(0),
    m_joinMode(false),
    m_wasAtEnd(false)
{
    ui->setupUi(this);

    // context menu for user list
    m_userMenu = new QMenu(this);

    //: Noun
    m_userMenu->addAction(ChatTools::chatIcon(), tr("Chat"), this, SLOT(slotStartChatFromMenu()));

    //: This is the label on a menu item that user clicks to issue the command. Means "Kick the selected user right now"
    m_kickNow = new QAction(tr("Kick now"), this);
    connect(m_kickNow, SIGNAL(triggered()), this, SLOT(slotKickNow()));

    //: This is the label on a menu item that user clicks to issue the command. Means "Kick the selected user with the following reason"
    m_kickWithReason = new QAction(tr("Kick with reason..."), this);
    connect(m_kickWithReason, SIGNAL(triggered()), this, SLOT(slotKickWithReason()));

    m_userMenu->addSeparator();
    m_userMenu->addAction(m_kickNow);
    m_userMenu->addAction(m_kickWithReason);

    //: This is the label on a menu item that user clicks to issue the command. Means "Ban the selected user right now"
    m_banNow = new QAction(tr("Ban now"), this);
    connect(m_banNow, SIGNAL(triggered()), this, SLOT(slotBanNow()));

    //: This is the label on a menu item that user clicks to issue the command. Means "Ban the selected user with the following reason"
    m_banWithReason = new QAction(tr("Ban with reason..."), this);
    connect(m_banWithReason, SIGNAL(triggered()), this, SLOT(slotBanWithReason()));

    m_userMenu->addSeparator();
    m_userMenu->addAction(m_banNow);
    m_userMenu->addAction(m_banWithReason);

    // context menu for room
    m_roomMenu = new QMenu(this);

    //: This is the label on a menu item that user clicks to issue the command. Means "Configure this room"
    m_configureRoom = new QAction(tr("Configure room") + "...", this);
    connect(m_configureRoom, SIGNAL(triggered()), this, SLOT(slotConfigureRoom()));

    //: This is the label on a menu item that user clicks to issue the command. Means "Copy the JID of the room to the clipboard"
    m_roomMenu->addAction(tr("Copy room JID"), this, SLOT(slotCopyRoomJid()));
    //: This is the label on a menu item that user clicks to issue the command. Means "Send invitations to the selected users"
    m_roomMenu->addAction(tr("Send invitations") + "...", this, SLOT(slotSendInvitations()));
    m_roomMenu->addSeparator();
    m_roomMenu->addAction(m_configureRoom);

    // General discussion
    m_splitter = new QSplitter(Qt::Horizontal, ui->tabsChats);

    m_generalPage = new ChatMessages(m_splitter);
    m_generalMessages = m_generalPage->messages();

    m_listUsers = new QListWidget(m_splitter);
    m_listUsers->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listUsers->setSortingEnabled(true);

    connect(m_listUsers, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotUserDoubleClicked(QListWidgetItem*)));
    connect(m_listUsers, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomContextMenuRequested(QPoint)));

    m_splitter->addWidget(m_generalPage);
    m_splitter->addWidget(m_listUsers);
    m_splitter->setCollapsible(0, false);

    //: Means "General chat"
    ui->tabsChats->addTab(m_splitter, tr("General"));

    connect(m_generalMessages, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotAnchorClicked(QUrl)));

    // hide tabbar
    m_bar = ui->tabsChats->findChild<QTabBar *>();
    m_bar->hide();

    // font size
    setFontSize(SETTINGS_GET_INT(SETTING_CHAT_FONT_SIZE));

    m_unreadMesagesAnimation = new ColorAnimation(ui->labelUnreadMessages, this);

    m_companyTemplate =
            QString("<br><table>")
            + "<tr><td>" + tr("Company:")        + "</td><td>%1</td></tr>"
            //: Noun. Means "Market exchange:", http://www.investopedia.com/terms/e/exchange.asp
            + "<tr><td>" + tr("Exchange:")       + "</td><td>%2</td></tr>"
            + "<tr><td>" + tr("Sector:")         + "</td><td>%3</td></tr>"
            + "<tr><td>" + tr("Industry:")       + "</td><td>%4</td></tr>"
            + "<tr><td>" + tr("Capitalization:") + "</td><td>%L5 %6</td></tr>"
            + "</table><br>";

    // exchange binds
    m_exchangeBinds.insert("N", "NYSE");
    m_exchangeBinds.insert("D", "NASD");
    m_exchangeBinds.insert("A", "AMEX");

    ui->plainMessage->installEventFilter(this);
    ui->lineRoom->setText(jid);
    ui->lineNick->setText(nick);
    ui->linePassword->setText(password);

    m_rxTickerInfo = QRegExp(QString("/(%1)").arg(Tools::tickerValidator().pattern()));
    m_rxIndustryInfo = QRegExp("//([a-zA-Z\\s&,\\-\\\\/]+)((?:=[nNdDaA]+)?)");
    m_rxOpenTicker = QRegExp(QString("=(%1)=(?=\\s|$)").arg(Tools::tickerValidator().pattern()));

    enableAdminActions(false);
    enableOwnerActions(false);

    setJoinMode(true);

    bool setDefaultSizes = true;

    if(checkForAutoLogin && SETTINGS_GET_BOOL(SETTING_CHAT_AUTO_LOGIN_TO_ROOMS))
    {
        if(jid.isEmpty())
        {
            QXmppStanza::Error error;
            error.setCondition(QXmppStanza::Error::JidMalformed);
            slotError(error);
        }
        else
        {
            QByteArray state = SETTINGS_GET_BYTE_ARRAY(ui->lineRoom->text() + THT_CHAT_SPLITTER_STATE_SUFFIX);

            if(!state.isEmpty())
            {
                setDefaultSizes = false;
                m_splitter->restoreState(state);
            }

            QTimer::singleShot(0, ui->pushJoin, SLOT(click()));
        }
    }

    if(setDefaultSizes)
    {
        // stretch factors
        QList<int> sizes;

        sizes.append(300);
        sizes.append(100);

        m_splitter->setSizes(sizes);
    }

    // context menu for subject lineedit
    QAction *a;
    //: This is the label on a menu item that user clicks to issue the command
    a = new QAction(tr("Set subject") + "...", ui->lineSubject);
    connect(a, SIGNAL(triggered()), this, SLOT(slotSetSubject()));
    ui->lineSubject->addAction(a);
}

ChatPage::~ChatPage()
{
    if(m_room && SETTINGS_GET_BOOL(SETTING_CHAT_AUTO_LOGIN_TO_ROOMS))
        SETTINGS_SET_BYTE_ARRAY(m_room->jid() + THT_CHAT_SPLITTER_STATE_SUFFIX, m_splitter->saveState());

    if(m_room)
        m_room->leave();

    delete m_room;
    m_room = 0;
    delete ui;
}

void ChatPage::slotMessageReceived(const QXmppMessage &msg)
{
    QPair<QString, QString> parsed = formatMessage(msg);

    if(parsed.first.isEmpty() || parsed.second.isEmpty())
        return;

    // show message or save in buffer
    if(m_joinMode)
    {
        m_unreadMessages.append(parsed.second);
        showUnreadMessagesCount();
    }
    else
    {
        ChatMessages *chatMessages;

        if(msg.type() == QXmppMessage::Chat)
            chatMessages = addPrivateChat(parsed.first, false);
        else
            chatMessages = m_generalPage;

        chatMessages->messages()->append(parsed.second);

        if(ui->tabsChats->currentWidget() != chatMessages)
            ui->tabsChats->setTabIcon(ui->tabsChats->indexOf(chatMessages), ChatTools::unreadIcon());
    }

    emit message();
}

void ChatPage::slotJoin()
{
    qDebug("Joining room \"%s\"", qPrintable(ui->lineRoom->text()));

    ui->pushJoin->setEnabled(false);
    ui->pushCancel->setEnabled(true);

    emit requestJoin(ui->lineRoom->text());
}

void ChatPage::slotCancel()
{
    deleteLater();
}

void ChatPage::slotLeft()
{
    qDebug("Left");

    setJoinMode(true);

    emit left();
}

void ChatPage::slotJoined()
{
    qDebug("Joined");

    setJoinMode(false);

    m_generalMessages->clear();
    ui->plainMessage->setEnabled(true);

    slotSubjectChanged(m_room->subject());
    slotAllowedActionsChanged();

    // clear unread messages
    foreach(QString s, m_unreadMessages)
    {
        m_generalMessages->append(s);
    }

    m_unreadMessages.clear();

    qDebug("Permissions request sent: %s", m_room->requestPermissions() ? "yes" : "no");

    emit joined(m_room->name());
}

void ChatPage::slotKicked(const QString &jid, const QString &reason)
{
    Q_UNUSED(jid)

    qDebug("Kicked");

    if(reason.isEmpty())
        QMessageBox::information(this, tr("Kicked"), tr("You have been kicked from \"%1\"").arg(roomName()));
    else
        QMessageBox::information(this, tr("Kicked"), tr("You have been kicked from \"%1\". Reason: %2").arg(roomName()).arg(reason));
}

void ChatPage::slotSubjectChanged(const QString &subject)
{
    ui->lineSubject->setText(subject);
    ui->lineSubject->setCursorPosition(0);
}

void ChatPage::slotError(const QXmppStanza::Error &error)
{
    QString s = errorToString(error);

    qDebug("Error \"%s\", join mode: %s", qPrintable(s), m_joinMode ? "yes" : "no");

    appendError(s);

    if(m_joinMode)
    {
        ui->pushJoin->setEnabled(true);
        ui->pushCancel->setEnabled(false);
    }
}

void ChatPage::slotAllowedActionsChanged()
{
    bool allowedToKick  = (m_room->allowedActions() & QXmppMucRoom::KickAction);

    if(allowedToKick)
        qDebug("Allowed to kick");
    else
        qDebug("Is NOT allowed to kick");

    enableKickActions(allowedToKick);
}

void ChatPage::slotParticipantAdded(const QString &jid)
{
    qDebug("Added user %s", qPrintable(jid));

    QString nick = jidToNick(jid);

    m_listUsers->addItem(nick);

    presenceChanged(m_room->participantPresence(jid));

    sendSystemMessageToPrivateChat(nick, tr("User is available"));
}

void ChatPage::slotParticipantRemoved(const QString &jid)
{
    qDebug("Removed user %s", qPrintable(jid));

    QString nick = jidToNick(jid);

    QList<QListWidgetItem *> items = m_listUsers->findItems(nick, Qt::MatchFixedString | Qt::MatchCaseSensitive);

    foreach(QListWidgetItem *i, items)
    {
        delete i;
    }

    sendSystemMessageToPrivateChat(nick, tr("User is not available"));
}

void ChatPage::slotPermissionsReceived(const QList<QXmppMucItem> &list)
{
    QString myJid = m_xmppClient->configuration().jidBare();

    foreach(QXmppMucItem i, list)
    {
        if(i.jid() == myJid)
        {
            enableAdminActions(i.affiliation() == QXmppMucItem::OwnerAffiliation || i.affiliation() == QXmppMucItem::AdminAffiliation);
            enableOwnerActions(i.affiliation() == QXmppMucItem::OwnerAffiliation);
        }
    }
}

void ChatPage::slotAnchorClicked(const QUrl &url)
{
    if(url.scheme() == "chat-user")
    {
        qDebug("Clicked: user");
        Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();

        if(mods == Qt::NoModifier)
        {
            ui->plainMessage->appendPlainText(url.userName() + ": ");
            ui->plainMessage->setFocus();
        }
        else if(mods == Qt::ControlModifier)
        {
            startPrivateChat(url.userName());
        }
    }
    else if(url.scheme() == "chat-ticker")
    {
        qDebug("Clicked: ticker");
        emit openTicker(url.userName());
    }
    else if(url.scheme() == "chat-industry")
    {
        qDebug("Clicked: industry");
        sendMessageToCurrentChat("//" + url.userName());
    }
    else
    {
        qDebug("Clicked: link");
        QDesktopServices::openUrl(url);
    }
}

void ChatPage::slotTabCloseRequested(int index)
{
    if(!index)
        return;

    delete ui->tabsChats->widget(index);

    if(ui->tabsChats->count() == 1)
        m_bar->hide();
}

void ChatPage::slotCurrentTabChanged(int index)
{
    QScrollBar *s = m_generalMessages->verticalScrollBar();

    if(s)
    {
        if(index > 0)
            m_wasAtEnd = (s->value() == s->maximum());
        else if(m_wasAtEnd)
            s->setValue(s->maximum());
    }

    ui->tabsChats->setTabIcon(index, QIcon());
}

void ChatPage::slotUnreadMessagesClicked()
{
    if(m_unreadMessages.isEmpty())
        return;

    QString m = m_unreadMessages.takeLast();
    showUnreadMessagesCount();

    MessageDialog msg(m, this);
    msg.exec();
}

void ChatPage::slotMessageDelivered(const QString &jid, const QString &id)
{
    qDebug("Delivery for jid \"%s\", id \"%s\"", qPrintable(jid), qPrintable(id));

    QHash<QString, QXmppMessage>::iterator it = m_undeliveredMessages.find(id);

    if(it == m_undeliveredMessages.end())
    {
        qWarning("Delivered id \"%s\" is not found", qPrintable(id));
        return;
    }

    QXmppMessage msg = it.value();
    m_undeliveredMessages.erase(it);

    msg.setFrom(m_room->nickName());
    QPair<QString, QString> parsed = formatMessage(msg);

    if(parsed.second.isEmpty())
        return;

    int index = 1;
    QString nick = jidToNick(jid);

    // do we chat with this user already?
    while(index < ui->tabsChats->count())
    {
        if(ui->tabsChats->tabText(index) == nick)
        {
            ChatMessages *chatMessages = qobject_cast<ChatMessages *>(ui->tabsChats->widget(index));
            chatMessages->messages()->append(parsed.second);
            break;
        }

        index++;
    }
}

void ChatPage::slotUserDoubleClicked(QListWidgetItem *item)
{
    if(!item)
        return;

    startPrivateChat(item->text());
}

void ChatPage::slotCustomContextMenuRequested(const QPoint &point)
{
    QListWidgetItem *item = m_listUsers->itemAt(point);

    if(!item)
        m_roomMenu->exec(m_listUsers->mapToGlobal(point));
    else
        m_userMenu->exec(m_listUsers->mapToGlobal(point));
}

void ChatPage::slotStartChatFromMenu()
{
    QListWidgetItem *item = m_listUsers->currentItem();

    if(!item)
        return;

    startPrivateChat(item->text());
}

void ChatPage::slotKickNow(const QString &reason)
{
    if(!m_room)
        return;

    QListWidgetItem *item = m_listUsers->currentItem();

    if(!item)
        return;

    QString jid = m_room->jid() + '/' + item->text();

    qDebug("Kick \"%s\"", qPrintable(jid));

    m_room->kick(jid, reason);
}

void ChatPage::slotKickWithReason()
{
    bool ok;
    QString reason = QInputDialog::getText(this,
                                           //: This is the label on a menu item that user clicks to issue the command. Means "Kick the selected user"
                                           tr("Kick"),
                                           tr("Reason:"), QLineEdit::Normal, QString(), &ok);

    if(!ok)
        return;

    slotKickNow(reason);
}

void ChatPage::slotBanNow(const QString &reason)
{
    if(!m_room)
        return;

    QListWidgetItem *item = m_listUsers->currentItem();

    if(!item)
        return;

    QString jid = m_room->participantFullJid(m_room->jid() + '/' + item->text());

    if(jid.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to find the JID of the user \"%1\"").arg(item->text()));
        return;
    }

    qDebug("Ban \"%s\"", qPrintable(jid));

    m_room->ban(QXmppUtils::jidToBareJid(jid), reason);
}

void ChatPage::slotBanWithReason()
{
    bool ok;
    QString reason = QInputDialog::getText(this,
                                           //: This is the label on a menu item that user clicks to issue the command. Means "Ban the selected user"
                                           tr("Ban"),
                                           tr("Reason:"), QLineEdit::Normal, QString(), &ok);

    if(!ok)
        return;

    slotBanNow(reason);
}

void ChatPage::slotConfigureRoom()
{
    ConfigureRoom cr(m_room, this);
    cr.exec();
}

void ChatPage::slotSetSubject()
{
    qDebug("Setting subject");

    bool ok;
    QString subject = QInputDialog::getText(this,
                                            tr("Set subject"),
                                            tr("Subject") + ':',
                                            QLineEdit::Normal,
                                            ui->lineSubject->text(),
                                            &ok);

    if(!ok)
        return;

    m_room->setSubject(subject);
}

void ChatPage::slotCopyRoomJid()
{
    if(m_room)
        QApplication::clipboard()->setText(m_room->jid());
}

void ChatPage::slotSendInvitations()
{
    SendInvitations sn(m_room, m_xmppClient, this);
    sn.exec();
}

QString ChatPage::roomName() const
{
    return m_room ? m_room->name() : QString();
}

QString ChatPage::jid() const
{
    return m_room ? m_room->jid() : ui->lineRoom->text();
}

QString ChatPage::nick() const
{
    return m_room ? m_room->nickName() : ui->lineNick->text();
}

QString ChatPage::password() const
{
    return m_room ? m_room->password() : ui->linePassword->text();
}

void ChatPage::proceedJoin()
{
    if(m_room)
        m_room->leave();

    delete m_room;
    m_room = m_muc->addRoom(ui->lineRoom->text());

    if(!m_room)
    {
        qDebug("Room is not added");
        return;
    }

    connect(m_room, SIGNAL(messageReceived(QXmppMessage)), this, SLOT(slotMessageReceived(QXmppMessage)));
    connect(m_room, SIGNAL(subjectChanged(QString)),       this, SLOT(slotSubjectChanged(QString)));
    connect(m_room, SIGNAL(left()),                        this, SLOT(slotLeft()));
    connect(m_room, SIGNAL(joined()),                      this, SLOT(slotJoined()));
    connect(m_room, SIGNAL(kicked(QString,QString)),       this, SLOT(slotKicked(QString,QString)));
    connect(m_room, SIGNAL(error(QXmppStanza::Error)),     this, SLOT(slotError(QXmppStanza::Error)));
    connect(m_room, SIGNAL(participantAdded(QString)),     this, SLOT(slotParticipantAdded(QString)));
    connect(m_room, SIGNAL(participantRemoved(QString)),   this, SLOT(slotParticipantRemoved(QString)));
    connect(m_room, SIGNAL(nameChanged(QString)),          this, SIGNAL(nameChanged(QString)));
    connect(m_room, SIGNAL(permissionsReceived(QList<QXmppMucItem>)),
            this, SLOT(slotPermissionsReceived(QList<QXmppMucItem>)));
    connect(m_room, SIGNAL(allowedActionsChanged(QXmppMucRoom::Actions)),
            this, SLOT(slotAllowedActionsChanged()));

    ui->labelStatus->clear();
    ui->plainMessage->setEnabled(false);

    if(ui->lineNick->text().isEmpty())
        ui->lineNick->setText(QXmppUtils::jidToUser(m_xmppClient->configuration().jidBare()));

    m_room->setNickName(ui->lineNick->text());
    m_room->setPassword(ui->linePassword->text());
    m_room->join();
}

void ChatPage::setFontSize(int size)
{
    if(size < 6)
        size = 6;

    QFont f = m_generalMessages->font();
    f.setPointSize(size);
    m_generalMessages->setFont(f);
}

void ChatPage::presenceChanged(const QXmppPresence &presence)
{
    if(!m_room)
        return;

    QString nick = QXmppUtils::jidToResource(presence.from());

    // user status
    QIcon icon = ChatTools::statusIcon(presence.availableStatusType());
    QList<QListWidgetItem *> items = m_listUsers->findItems(nick, Qt::MatchFixedString | Qt::MatchCaseSensitive);

    foreach(QListWidgetItem *i, items)
    {
        i->setIcon(icon);
    }

    if(nick == m_room->nickName())
    {
        const QXmppMucItem::Affiliation a = presence.mucItem().affiliation();
        enableAdminActions(a == QXmppMucItem::OwnerAffiliation || a == QXmppMucItem::AdminAffiliation);
        enableOwnerActions(a == QXmppMucItem::OwnerAffiliation);
    }
}

bool ChatPage::isJoined() const
{
    return m_room ? m_room->isJoined() : false;
}

bool ChatPage::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        bool ate = true;

        if(ke->key() == Qt::Key_Return)
        {
            if(ke->modifiers() == Qt::NoModifier)
            {
                m_lastMessage = ui->plainMessage->toPlainText();
                sendMessageToCurrentChat(m_lastMessage);
                ui->plainMessage->clear();
            }
            else if(ke->modifiers() == Qt::ShiftModifier)
                ui->plainMessage->insertPlainText("\n");
            else
                ate = false;
        }
        else if(ke->key() == Qt::Key_Up && ui->plainMessage->document()->isEmpty())
            ui->plainMessage->appendPlainText(m_lastMessage);
        else
            ate = false;

        if(ate)
            return true;
    }

    return QWidget::eventFilter(obj, event);
}

void ChatPage::setJoinMode(bool jm)
{
    if(m_joinMode == jm)
        return;

    m_joinMode = jm;
    ui->stack->setCurrentIndex(!m_joinMode);

    if(m_joinMode)
    {
        ui->pushJoin->setEnabled(true);
        ui->pushCancel->setEnabled(false);
        ui->labelStatus->clear();
        showUnreadMessagesCount();
        QTimer::singleShot(0, ui->lineRoom, SLOT(setFocus()));
    }
    else
        QTimer::singleShot(0, ui->plainMessage, SLOT(setFocus()));
}

void ChatPage::appendError(const QString &s)
{
    QString msg = "<font color=red><b>" + s + "</b></font>";

    if(m_joinMode)
        ui->labelStatus->setText(msg);
    else
        m_generalMessages->append(msg);
}

QString ChatPage::errorToString(const QXmppStanza::Error &error) const
{
    switch(error.condition())
    {
        case QXmppStanza::Error::BadRequest:            return tr("Bad request");
        case QXmppStanza::Error::Conflict:              return tr("User conflict");
        case QXmppStanza::Error::FeatureNotImplemented: return tr("Feature is not implemented");
        case QXmppStanza::Error::Forbidden:             return tr("Forbidden");
        case QXmppStanza::Error::Gone:                  return tr("User has gone");
        case QXmppStanza::Error::InternalServerError:   return tr("Internal server error");
        case QXmppStanza::Error::ItemNotFound:          return tr("Item is not found");
        //: JID is a Jabber Identifier, http://en.wikipedia.org/wiki/XMPP#Decentralization_and_addressing . It's ok not to translate "JID", e.g. you can just copy-paste "JID" to your translation
        case QXmppStanza::Error::JidMalformed:          return tr("JID is malformed");
        case QXmppStanza::Error::NotAcceptable:         return tr("Not acceptable");
        case QXmppStanza::Error::NotAllowed:            return tr("Not allowed");
        case QXmppStanza::Error::NotAuthorized:         return tr("Not authorized");
        case QXmppStanza::Error::PaymentRequired:       return tr("Payment required");
        case QXmppStanza::Error::RecipientUnavailable:  return tr("Recipient is unavailable");
        case QXmppStanza::Error::Redirect:              return tr("Redirection");
        case QXmppStanza::Error::RegistrationRequired:  return tr("Registration required");
        case QXmppStanza::Error::RemoteServerNotFound:  return tr("Remote server is not found");
        case QXmppStanza::Error::RemoteServerTimeout:   return tr("Remote server timeout");
        case QXmppStanza::Error::ResourceConstraint:    return tr("Resource constraint");
        case QXmppStanza::Error::ServiceUnavailable:    return tr("Service is unavailable");
        case QXmppStanza::Error::SubscriptionRequired:  return tr("Subscription required");
        case QXmppStanza::Error::UndefinedCondition:    return tr("Undefined condition");
        case QXmppStanza::Error::UnexpectedRequest:     return tr("Unexpected request");

        default:
            return tr("Unknown error");
    }
}

void ChatPage::showUnreadMessagesCount()
{
    if(m_unreadMessages.isEmpty())
    {
        ui->labelUnreadMessages->clear();
        blinkUnreadMessages(false);
    }
    else
    {
        //: Means "Number of messages:"
        ui->labelUnreadMessages->setText(tr("Messages: %1").arg(m_unreadMessages.count()));
        blinkUnreadMessages(true);
    }
}

void ChatPage::blinkUnreadMessages(bool blink)
{
    if(blink)
        m_unreadMesagesAnimation->start();
    else
        m_unreadMesagesAnimation->stop();
}

QString ChatPage::tickerToLink(const QString &ticker) const
{
    QString upper = ticker.toUpper();

    return QString("<a href=\"chat-ticker://%1@\">%2</a>")
            .arg(escapeDog(upper))
            .arg(upper);
}

ChatMessages *ChatPage::addPrivateChat(const QString &nick, bool switchTo)
{
    int index = 1;

    // do we chat with this user already?
    while(index < ui->tabsChats->count())
    {
        if(ui->tabsChats->tabText(index) == nick)
        {
            if(switchTo)
                ui->tabsChats->setCurrentIndex(index);

            return qobject_cast<ChatMessages *>(ui->tabsChats->widget(index));
        }

        index++;
    }

    // not found, create a new one
    ChatMessages *chatMessages = new ChatMessages(ui->tabsChats);
    connect(chatMessages->messages(), SIGNAL(anchorClicked(QUrl)), this, SLOT(slotAnchorClicked(QUrl)));
    index = ui->tabsChats->addTab(chatMessages, nick);

    if(switchTo)
        ui->tabsChats->setCurrentIndex(index);

    m_bar->show();

    if(m_listUsers->findItems(nick, Qt::MatchFixedString | Qt::MatchCaseSensitive).isEmpty())
        sendSystemMessageToPrivateChat(nick, tr("User is not available"));

    return chatMessages;
}

void ChatPage::startPrivateChat(const QString &nick)
{
    if(nick != m_room->nickName())
    {
        qDebug("Starting private chat");
        addPrivateChat(nick);
        ui->plainMessage->setFocus();
    }
}

QPair<QString, QString> ChatPage::formatMessage(const QXmppMessage &msg)
{
    // construct nick
    QString nick =
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
            jidToNick(msg.from()).toHtmlEscaped();
#else
            Qt::escape(jidToNick(msg.from()));
#endif

    if(nick.isEmpty())
    {
        qWarning("Nick is empty");
        return QPair<QString, QString>();
    }

    // timestamp
    QDateTime stamp;

    if(msg.stamp().isValid())
        stamp = msg.stamp().toTimeSpec(Qt::LocalTime);
    else
        stamp = QDateTime::currentDateTime();

    QString color = ChatTools::randomColor().name();
    QString body;

    // error message?
    if(msg.error().code())
    {
        QString errorText =
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                msg.error().text().toHtmlEscaped();
#else
                Qt::escape(msg.error().text());
#endif

        body = "<font color=red><b>"
                + errorToString(msg.error())
                + (errorText.isEmpty() ? QString() : (" (" + errorText + ')'))
                + "</b></font>";

        QHash<QString, QXmppMessage>::iterator it = m_undeliveredMessages.find(msg.id());

        // replace the undelivered private message with a new one
        if(it != m_undeliveredMessages.end())
        {
            QXmppMessage newMsg(msg.from(), QString(), body);
            newMsg.setError(msg.error());
            it.value() = newMsg;
            return QPair<QString, QString>();
        }

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        nick = m_room->jid().toHtmlEscaped();
#else
        nick = Qt::escape(m_room->jid());
#endif
    }
    else
    {
        body = ChatTools::escapeBrackets(msg.body());

        if(body.isEmpty())
        {
            body = ChatTools::escapeBrackets(msg.subject());

            if(body.isEmpty())
                return QPair<QString, QString>();

            body = "*** " + tr("New subject:") + ' ' + body + " ***";
        }

        // tickers from the industry
        if(m_rxIndustryInfo.exactMatch(body))
        {
            QString industry = m_rxIndustryInfo.cap(1);
            QString exchange =  m_rxIndustryInfo.cap(2).toUpper().mid(1);
            QMap<QString, QString> binds;
            QStringList exchangesAppend;

            binds.insert(":industry", industry);

            if(!exchange.isEmpty())
            {
                QMap<QString, QString> exchangeBinds;
                QStringList queries;
                QString seen;
                int q = 0;

                foreach(QChar ch, exchange)
                {
                    if(seen.indexOf(ch) >= 0)
                        continue;

                    seen.append(ch);

                    QHash<QString, QString>::iterator it = m_exchangeBinds.find(QString(ch));

                    if(it != m_exchangeBinds.end())
                    {
                        QString s = QString("exchange%1").arg(q++);
                        queries += QString("exchange = :%1").arg(s);
                        exchangesAppend.append(it.value());
                        exchangeBinds.insert(QString(":%1").arg(s), it.value());
                    }
                }

                if(queries.isEmpty())
                    exchange.clear();
                else
                {
                    // all the exchanges?
                    if(seen.size() == m_exchangeBinds.size())
                    {
                        exchange.clear();
                        exchangesAppend.clear();
                    }
                    else
                    {
                        exchange = "AND ( " + queries.join(" OR ") + " )";

                        // append binds
                        QMapIterator<QString, QString> i(exchangeBinds);

                        while(i.hasNext())
                        {
                            i.next();
                            binds.insert(i.key(), i.value());
                        }
                    }
                }
            }

            bool ok = false;

            QList<QVariantList> lists = SqlTools::query(
                                            QString("SELECT ticker FROM tickers WHERE industry = :industry %1 ORDER BY cap DESC")
                                                    .arg(exchange),
                                            binds);

            if(!lists.isEmpty())
            {
                ok = true;
                body = industry + (exchangesAppend.isEmpty() ? QString() : (' ' + exchangesAppend.join(","))) + ": ";

                foreach(QVariantList values, lists)
                {
                    if(values.isEmpty())
                        continue;

                    body += QString(" =%1= ").arg(values.at(0).toString());
                }
            }

            if(!ok)
                body = industry + ": " + tr("industry is not found");
        }

        // ticker info
        if(m_rxTickerInfo.exactMatch(body))
        {
            QString ticker = m_rxTickerInfo.cap(1).toUpper();
            bool ok = false;

            QList<QVariantList> lists = SqlTools::query(
                                            "SELECT company, exchange, sector, industry, cap FROM tickers WHERE ticker = :ticker",
                                            ":ticker",
                                            ticker);

            if(!lists.isEmpty())
            {
                QVariantList values = lists.at(0);

                // must be 5 values
                if(values.size() == 5)
                {
                    QString company = values.at(0).toString();

                    if(!company.isEmpty())
                    {
                        ok = true;
                        double cap = values.at(4).toDouble();
                        QString industry = values.at(3).toString();
                        QString capRank;

                        if(cap > 1000)
                        {
                            cap /= 1000;
                            //: Means "billion" (1000*million)
                            capRank = tr("bln");
                        }
                        else
                            //: Means "million"
                            capRank = tr("mln");

                        body = tickerToLink(ticker)
                                + ':'
                                + m_companyTemplate
                                                .arg(company)
                                                .arg(values.at(1).toString())
                                                .arg(values.at(2).toString())
                                                .arg(QString("<a href=\"chat-industry://%1@\">%2</a>")
                                                     .arg(escapeDog(industry))
                                                     .arg(industry)
                                                    )
                                                .arg(cap, 0, 'f', cap > 100 ? 1 : 2)
                                                .arg(capRank);
                    }
                }
            }

            if(!ok)
                //: Ticker is a short company name, see http://www.investopedia.com/terms/t/tickersymbol.asp . The common practice is to borrow "ticker" from English and incorporate into your language (see http://en.wikipedia.org/wiki/Loanword)
                body = ticker + ": " + tr("ticker is not found");
        }
        else
        {
            int pos = 0;
            QString res;

            // replace links
            body.replace(ChatTools::urlRegExp(), "<a href='\\1'>\\1</a>");

            // replace "=ABC=" with link which will open ABC in the linked windows
            while((pos = m_rxOpenTicker.indexIn(body, pos)) != -1)
            {
                if(!pos || body.at(pos-1).isSpace())
                {
                    res = tickerToLink(m_rxOpenTicker.cap(1));
                    body.replace(pos, m_rxOpenTicker.matchedLength(), res);
                    pos += res.length();
                }
                else
                    pos += m_rxOpenTicker.matchedLength();
            }
        }
    }

    body.replace("\n", "<br>");

    return QPair<QString, QString>(
            nick,
            (QString("<font color=\"")
                + color
                + "\">"
                + (SETTINGS_GET_BOOL(SETTING_CHAT_SHOW_TIME)
                    ? ('[' + stamp.toString("hh:mm:ss") + ']')
                    : QString())
                + QString(" <a class=\"%1\" href=\"chat-user://").arg(QString(color).replace(0, 1, 'c'))
                + escapeDog(nick)
                + "@\">"
                + nick
                + "</a>:</font> "
                + body));
}

QPair<QString, QString> ChatPage::formatSystemMessage(const QString &message)
{
    return formatMessage(QXmppMessage(m_room->jid(), QString(), "*** " + message + " ***"));
}

void ChatPage::sendSystemMessageToPrivateChat(const QString &nick, const QString &message)
{
    int index = 1;

    // do we chat with this user already?
    while(index < ui->tabsChats->count())
    {
        if(ui->tabsChats->tabText(index) == nick)
        {
            ChatMessages *chatMessages = qobject_cast<ChatMessages *>(ui->tabsChats->widget(index));

            chatMessages->messages()->append(formatSystemMessage(message).second);

            break;
        }
    }
}

void ChatPage::sendMessageToCurrentChat(const QString &text)
{
    if(!ui->tabsChats->currentIndex())
        m_room->sendMessage(text);
    else
    {
        QString jid = m_room->jid() + '/' + ui->tabsChats->tabText(ui->tabsChats->currentIndex());

        QXmppMessage msg(QString(), jid, text);
        msg.setReceiptRequested(true);
        m_undeliveredMessages[msg.id()] = msg;
        m_xmppClient->sendPacket(msg);
    }
}

void ChatPage::enableKickActions(bool enab)
{
    m_kickNow->setEnabled(enab);
    m_kickWithReason->setEnabled(enab);
}

void ChatPage::enableAdminActions(bool enab)
{
    m_banNow->setEnabled(enab);
    m_banWithReason->setEnabled(enab);
}

void ChatPage::enableOwnerActions(bool enab)
{
    m_configureRoom->setEnabled(enab);
}
