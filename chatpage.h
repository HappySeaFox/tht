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

#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include <QColor>
#include <QList>

#include "QXmppMucManager.h"
#include "QXmppStanza.h"

class QUrl;

class QXmppMucManager;
class QXmppMucItem;
class QXmppMessage;

class ColorAnimation;

namespace Ui
{
    class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QXmppMucManager *manager,
                      bool checkForAutoLogin,
                      const QString &jid = QString(),
                      const QString &password = QString(),
                      QWidget *parent = 0);
    ~ChatPage();

    QString roomName() const;

    QString jid() const;
    QString password() const;

    void proceedJoin();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    void setJoinMode(bool);
    void appendError(const QString &);
    QString errorToString(const QXmppStanza::Error &error);
    void showUnreadMessagesCount();
    void blinkUnreadMessages(bool);
    QString tickerToLink(const QString &) const;

signals:
    void requestJoin(const QString &jid);
    void joined(const QString &roomName = QString());
    void left();
    void nameChanged(const QString &);
    void message();
    void openTicker(const QString &);

private slots:
    void slotMessageReceived(const QXmppMessage &);
    void slotJoin();
    void slotCancel();
    void slotLeft();
    void slotJoined();
    void slotKicked(const QString &jid, const QString &reason);
    void slotSubjectChanged(const QString &subject);
    void slotError(const QXmppStanza::Error &);
    void slotAllowedActionsChanged(QXmppMucRoom::Actions);
    void slotParticipantAdded(const QString &);
    void slotParticipantRemoved(const QString &);
    void slotPermissionsReceived(const QList<QXmppMucItem> &);
    void slotAnchorClicked(const QUrl &);
    void slotUnreadMessagesClicked();

private:
    Ui::ChatPage *ui;
    QXmppMucManager *m_muc;
    QXmppMucRoom *m_room;
    QXmppMucRoom::Actions m_actions;
    bool m_joinMode;
    QRegExp m_rxTickerInfo, m_rxOpenTicker;
    QString m_lastMessage;
    QStringList m_unreadMessages;
    ColorAnimation *m_unreadMesagesAnimation;
    QString m_companyTemplate;
    QList<QColor> m_colors;
};

#endif // CHATPAGE_H
