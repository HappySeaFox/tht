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
#include <QMessageBox>
#include <QDateTime>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>
#include <QUrl>
#include <Qt>

#include <ctime>

#include "QXmppMessage.h"
#include "QXmppUtils.h"
#include "QXmppMucIq.h"

#include "coloranimation.h"
#include "messagedialog.h"
#include "chatsettings.h"
#include "settings.h"
#include "chatpage.h"
#include "sqltools.h"
#include "ui_chatpage.h"

ChatPage::ChatPage(QXmppMucManager *manager,
                   bool checkForAutoLogin,
                   const QString &jid,
                   const QString &password,
                   QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatPage),
    m_muc(manager),
    m_room(0),
    m_actions(0),
    m_joinMode(false)
{
    ui->setupUi(this);

    m_unreadMesagesAnimation = new ColorAnimation(ui->labelUnreadMessages, this);

    m_companyTemplate =
            QString("<br><table>")
            + "<tr><td>" + tr("Company:")        + "</td><td>%1</td></tr>"
            + "<tr><td>" + tr("Exchange:")       + "</td><td>%2</td></tr>"
            + "<tr><td>" + tr("Sector:")         + "</td><td>%3</td></tr>"
            + "<tr><td>" + tr("Industry:")       + "</td><td>%4</td></tr>"
            + "<tr><td>" + tr("Capitalization:") + "</td><td>%5 " + tr("mln") + "</td></tr>"
            + "</table><br>";

    m_colors << QColor(255, 0, 0)
             << QColor(128, 9, 0)
             << QColor(0, 255, 0)
             << QColor(0, 128, 0)
             << QColor(0, 0, 255)
             << QColor(0, 0, 128)
             << QColor(0, 128, 128)
             << QColor(255, 0, 255)
             << QColor(128, 0, 128)
             << QColor(128, 128, 0)
             << QColor(160, 160, 164)
             << QColor(128, 128, 128)
             << QColor(222, 0, 0);

    // NOTE
    // http://www.qtcentre.org/wiki/index.php?title=QTextBrowser_with_images_and_CSS

    qsrand(time(0) + QCoreApplication::applicationPid());

    ui->plainMessage->installEventFilter(this);
    ui->lineRoom->setText(jid);
    ui->linePassword->setText(password);

    m_rxTickerInfo = QRegExp(QString("/(%1)").arg(Settings::instance()->tickerValidator().pattern()));
    m_rxOpenTicker = QRegExp(QString("\\B=(%1)=(?=\\s|$)").arg(Settings::instance()->tickerValidator().pattern()));

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
    if(msg.body().isEmpty())
        return;

    QString nick = msg.from();
    nick = Qt::escape(nick.right(nick.length() - m_room->jid().length() - 1));

    QDateTime stamp;

    if(msg.stamp().isValid())
        stamp = msg.stamp().toTimeSpec(Qt::LocalTime);
    else
        stamp = QDateTime::currentDateTime();

    QString body;

    if(msg.error().code())
    {
        body = "<font color=red><b>" + errorToString(msg.error()) + "</b></font>";
    }
    else
    {
        body = Qt::escape(msg.body());

        // ticker info
        if(m_rxTickerInfo.exactMatch(body))
        {
            QString ticker = m_rxTickerInfo.cap(1).toUpper();
            bool ok = false;

            // TODO company name
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

                        body = tickerToLink(ticker)
                                + ':'
                                + m_companyTemplate
                                                .arg(company)
                                                .arg(values.at(1).toString())
                                                .arg(values.at(2).toString())
                                                .arg(values.at(3).toString())
                                                .arg(QString::number(values.at(4).toDouble(), 'f', 1));
                    }
                }
            }

            if(!ok)
                body = ticker + ": " + tr("not found");
        }
        else
        {
            int pos = 0;

            // replace "=ABC=" with link which will open ABC in the linked windows
            while((pos = m_rxOpenTicker.indexIn(body, pos)) != -1)
            {
                body.replace(pos, m_rxOpenTicker.matchedLength(), tickerToLink(m_rxOpenTicker.cap(1)));

                pos += m_rxOpenTicker.matchedLength();
            }
        }
    }

    body.replace("\n", "<br>");

    QString color = m_colors.at(qrand() % m_colors.size()).name();

    QString msgToAdd = QString('[')
                        + "<font color=\"" + color + "\">" + stamp.toString("hh:mm:ss") + "</font>"
                        + "] &lt;<a href=\"chat-user://"
                        + QString(nick).replace('@', "%40")
                        + "@\">"
                        + nick
                        + "</a>&gt;: "
                        + body;

    // show message or save in buffer
    if(m_joinMode)
    {
        m_unreadMessages.append(msgToAdd);
        showUnreadMessagesCount();
    }
    else
        ui->textMessages->append(msgToAdd);

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

    ui->textMessages->clear();
    ui->plainMessage->setEnabled(true);

    slotSubjectChanged(m_room->subject());
    slotAllowedActionsChanged(m_room->allowedActions());

    // clear unread messages
    foreach(QString s, m_unreadMessages)
    {
        ui->textMessages->append(s);
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
}

void ChatPage::slotParticipantRemoved(const QString &jid)
{
    qDebug("Removed user %s", qPrintable(jid));
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
        ui->plainMessage->appendPlainText(url.userName() + ": ");
        ui->plainMessage->setFocus();
    }
    else if(url.scheme() == "chat-open-ticker")
    {
        qDebug("Clicked: ticker");
        emit openTicker(url.userName());
    }
    else
    {
        qDebug("Clicked: link");
        QDesktopServices::openUrl(url);
    }
}

void ChatPage::slotUnreadMessagesClicked()
{
    if(m_unreadMessages.isEmpty())
        return;

    MessageDialog msg(m_unreadMessages.last(), this);
    msg.exec();

    m_unreadMessages.removeLast();
    showUnreadMessagesCount();
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

    m_room->setNickName("xxx13");
    m_room->setPassword(ui->linePassword->text());
    m_room->join();
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
                m_room->sendMessage(m_lastMessage);
                ui->plainMessage->clear();
            }
            else if(ke->modifiers() == Qt::ControlModifier)
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
        ui->textMessages->append("<font color=red><b>" + s + "</b></font>");
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
    return QString("<a href=\"chat-open-ticker://%1@\">%2</a>")
            .arg(QString(ticker).replace('@', "%40"))
            .arg(ticker);
}
