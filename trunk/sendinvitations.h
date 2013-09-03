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

#ifndef SENDINVITATIONS_H
#define SENDINVITATIONS_H

#include <QDialog>

class QXmppMucRoom;
class QXmppClient;

namespace Ui
{
    class SendInvitations;
}

class SendInvitations : public QDialog
{
    Q_OBJECT

public:
    explicit SendInvitations(QXmppMucRoom *room, QXmppClient *client, QWidget *parent = 0);
    ~SendInvitations();

private slots:
    void slotSend();
    void slotHelpMessage();

private:
    Ui::SendInvitations *ui;
    QXmppMucRoom *m_room;
    QXmppClient *m_xmppClient;
};

#endif // SENDINVITATIONS_H
