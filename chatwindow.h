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

#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include "QXmppClient.h"

#include <QWidget>

class QMenu;

class QXmppMucManager;

class ChatPage;

namespace Ui
{
    class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = 0);
    ~ChatWindow();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *);

private:
    void showSignInPage();
    void showChatsPage();
    void showLoginStatus(const QString &);
    void setTabName(QWidget *tab, const QString &roomName);
    ChatPage *createPage(bool checkForAutoLogin, const QString &jid = QString(), const QString &password = QString());
    void saveRooms();
    void restoreRooms();
    void removeTabs();
    bool chatsPage();

signals:
    void openTicker(const QString &);

private slots:
    void slotSignIn();
    void slotCancelSignIn();
    void slotError(QXmppClient::Error = QXmppClient::SocketError);
    void slotConnected();
    void slotDisconnected();
    void slotAddTab();
    void slotTabCloseRequested(int);
    void slotJoined(const QString &roomName);
    void slotLeft();
    void slotNameChanged(const QString &roomName);
    void slotJoinRequested(const QString &jid);
    void slotCurrentTabChanged(int);
    void slotMessage();
    void slotOptions();

private:
    Ui::ChatWindow *ui;
    QXmppClient *m_xmppClient;
    QXmppMucManager *m_muc;
    QMenu *m_menu;
};

#endif // CHATWINDOW_H
