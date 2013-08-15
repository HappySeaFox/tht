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
#include <QTextDocument>
#include <QApplication>
#include <QTextBrowser>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QKeyEvent>
#include <QTabBar>
#include <QTimer>
#include <QDebug>
#include <QUrl>
#include <Qt>

#include "QXmppConfiguration.h"
#include "QXmppMessage.h"
#include "QXmppClient.h"
#include "QXmppUtils.h"
#include "QXmppMucIq.h"

#include "coloranimation.h"
#include "messagedialog.h"
#include "chatsettings.h"
#include "chatmessages.h"
#include "chattools.h"
#include "settings.h"
#include "chatpage.h"
#include "sqltools.h"
#include "ui_chatpage.h"

ChatPage::ChatPage(QXmppClient *client,
                   QXmppMucManager *manager,
                   bool checkForAutoLogin,
                   const QString &jid,
                   const QString &password,
                   QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatPage),
    m_xmppClient(client),
    m_muc(manager),
    m_room(0),
    m_actions(0),
    m_joinMode(false),
    m_wasAtEnd(false)
{
    ui->setupUi(this);

    // General discussion
    m_generalPage = new ChatMessages(ui->tabsChats);
    m_generalMessages = m_generalPage->messages();

    ui->tabsChats->addTab(m_generalPage, tr("General"));

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
            + "<tr><td>" + tr("Exchange:")       + "</td><td>%2</td></tr>"
            + "<tr><td>" + tr("Sector:")         + "</td><td>%3</td></tr>"
            + "<tr><td>" + tr("Industry:")       + "</td><td>%4</td></tr>"
            + "<tr><td>" + tr("Capitalization:") + "</td><td>%L5 " + tr("mln") + "</td></tr>"
            + "</table><br>";

    ui->plainMessage->installEventFilter(this);
    ui->lineRoom->setText(jid);
    ui->linePassword->setText(password);

    m_rxTickerInfo = QRegExp(QString("/(%1)").arg(Settings::instance()->tickerValidator().pattern()));
    m_rxIndustryInfo = QRegExp("//(.*)");
    m_rxOpenTicker = QRegExp(QString("=(%1)=(?=\\s|$)").arg(Settings::instance()->tickerValidator().pattern()));

    setJoinMode(true);

    if(checkForAutoLogin && SETTINGS_GET_BOOL(SETTING_CHAT_AUTO_LOGIN_TO_ROOMS))
    {
        if(jid.isEmpty())
        {
            QXmppStanza::Error error;
            error.setCondition(QXmppStanza::Error::JidMalformed);
            slotError(error);
        }
        else
            QTimer::singleShot(0, ui->pushJoin, SLOT(click()));
    }
}

ChatPage::~ChatPage()
{
    if(m_room)
        m_room->leave();

    delete m_room;
    delete ui;
}

void ChatPage::slotMessageReceived(const QXmppMessage &msg)
{
    QStringList parsed = formatMessage(msg);

    if(parsed.size() < 2)
        return;

    // show message or save in buffer
    if(m_joinMode)
    {
        m_unreadMessages.append(parsed.at(1));
        showUnreadMessagesCount();
    }
    else
    {
        ChatMessages *chatMessages;

        if(msg.type() == QXmppMessage::Chat)
            chatMessages = addPrivateChat(parsed.at(0), false);
        else
            chatMessages = m_generalPage;

        chatMessages->messages()->append(parsed.at(1));

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
    slotAllowedActionsChanged(m_room->allowedActions());

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

void ChatPage::slotAllowedActionsChanged(QXmppMucRoom::Actions actions)
{
    m_actions = actions;

    if(m_actions & QXmppMucRoom::KickAction)
        qDebug("Allowed to kick");
    else
        qDebug("Is NOT allowed to kick");
}

void ChatPage::slotParticipantAdded(const QString &jid)
{
    qDebug("Added user %s", qPrintable(jid));

    QString nick = jidToNick(jid);

    m_users.append(nick);

    sendSystemMessageToPrivateChat(nick, tr("User is available"));
}

void ChatPage::slotParticipantRemoved(const QString &jid)
{
    qDebug("Removed user %s", qPrintable(jid));

    QString nick = jidToNick(jid);

    m_users.removeAll(nick);

    sendSystemMessageToPrivateChat(nick, tr("User is not available"));
}

void ChatPage::slotPermissionsReceived(const QList<QXmppMucItem> &list)
{
    foreach(QXmppMucItem i, list)
    {
        qDebug() << "PERMISSION" << i.jid() << QXmppMucItem::affiliationToString(i.affiliation()) << QXmppMucItem::roleToString(i.role());
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
            if(url.userName() != m_room->nickName())
            {
                qDebug("Starting private chat");
                addPrivateChat(url.userName());
                ui->plainMessage->setFocus();
            }
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
    msg.setFrom(m_room->nickName());
    QStringList parsed = formatMessage(msg);

    m_undeliveredMessages.remove(id);

    if(parsed.size() > 1)
    {
        int index = 1;
        QString nick = jidToNick(jid);
        ChatMessages *chatMessages = 0;

        // do we chat with this user already?
        while(index < ui->tabsChats->count())
        {
            if(ui->tabsChats->tabText(index) == nick)
            {
                chatMessages = qobject_cast<ChatMessages *>(ui->tabsChats->widget(index));
                break;
            }

            index++;
        }

        if(chatMessages)
            chatMessages->messages()->append(parsed.at(1));
    }
}

QString ChatPage::roomName() const
{
    return m_room ? m_room->name() : QString();
}

QString ChatPage::jid() const
{
    return m_room ? m_room->jid() : ui->lineRoom->text();
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
            this, SLOT(slotAllowedActionsChanged(QXmppMucRoom::Actions)));

    ui->labelStatus->clear();
    ui->plainMessage->setEnabled(false);

    m_room->setNickName(QXmppUtils::jidToUser(m_xmppClient->configuration().jidBare()));
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
    //m_generalMessages->scroll
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
    if(m_joinMode)
        ui->labelStatus->setText("<font color=red><b>" + s + "</b></font>");
    else
        m_generalMessages->append("<font color=red><b>" + s + "</b></font>");
}

QString ChatPage::errorToString(const QXmppStanza::Error &error)
{
    switch(error.condition())
    {
        case QXmppStanza::Error::BadRequest:            return tr("Bad request");
        case QXmppStanza::Error::Conflict:              return tr("User conflict");
        case QXmppStanza::Error::FeatureNotImplemented: return tr("Feature not implemented");
        case QXmppStanza::Error::Forbidden:             return tr("Forbidden");
        case QXmppStanza::Error::Gone:                  return tr("Gone");
        case QXmppStanza::Error::InternalServerError:   return tr("Internal server error");
        case QXmppStanza::Error::ItemNotFound:          return tr("Item not found");
        case QXmppStanza::Error::JidMalformed:          return tr("JID is malformed");
        case QXmppStanza::Error::NotAcceptable:         return tr("Not acceptable");
        case QXmppStanza::Error::NotAllowed:            return tr("Not allowed");
        case QXmppStanza::Error::NotAuthorized:         return tr("Not authorized");
        case QXmppStanza::Error::PaymentRequired:       return tr("Payment required");
        case QXmppStanza::Error::RecipientUnavailable:  return tr("Recipient is unavailable");
        case QXmppStanza::Error::Redirect:              return tr("Redirect");
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

    if(m_users.indexOf(nick) < 0)
        sendSystemMessageToPrivateChat(nick, tr("User is not available"));

    return chatMessages;
}

QStringList ChatPage::formatMessage(const QXmppMessage &msg)
{
    // construct nick
    QString nick = Qt::escape(jidToNick(msg.from()));

    if(nick.isEmpty())
    {
        qWarning("Nick is empty");
        return QStringList();
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
        nick = Qt::escape(m_room->jid());
        body = "<font color=red><b>" + errorToString(msg.error()) + "</b></font>";
    }
    else
    {
        body = Qt::escape(msg.body());

        if(body.isEmpty())
            return QStringList();

        // tickers from the industry
        if(m_rxIndustryInfo.exactMatch(body))
        {
            QString industry = m_rxIndustryInfo.cap(1);
            bool ok = false;

            QList<QVariantList> lists = SqlTools::query(
                                            "SELECT ticker FROM tickers WHERE industry = :industry ORDER BY cap DESC",
                                            ":industry",
                                            industry);

            if(!lists.isEmpty())
            {
                ok = true;
                body = industry + ": ";

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
                                                .arg(cap, 0, 'f', cap > 100 ? 0 : 1);
                    }
                }
            }

            if(!ok)
                body = ticker + ": " + tr("ticker is not found");
        }
        else
        {
            int pos = 0;
            QString res;

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

    return QStringList()
            << nick
            << (QString("<font color=\"")
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
                + body);
}

QStringList ChatPage::formatSystemMessage(const QString &message)
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

            QStringList parsed = formatSystemMessage(message);

            if(parsed.size() > 1)
                chatMessages->messages()->append(parsed.at(1));

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
