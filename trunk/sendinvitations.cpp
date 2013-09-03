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

#include <QStringList>

#include "sendinvitations.h"
#include "ui_sendinvitations.h"

#include "QXmppMucManager.h"
#include "QXmppMessage.h"
#include "QXmppClient.h"

SendInvitations::SendInvitations(QXmppMucRoom *room, QXmppClient *client, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendInvitations),
    m_room(room),
    m_xmppClient(client)
{
    ui->setupUi(this);
}

SendInvitations::~SendInvitations()
{
    delete ui;
}

void SendInvitations::slotSend()
{
    QStringList jids = ui->plainTextEdit->toPlainText().split('\n', QString::SkipEmptyParts);

    if(jids.isEmpty())
        return;

    qDebug("Send invitations");

    ui->pushSend->setEnabled(false);

    foreach(QString jid, jids)
    {
        QXmppMessage message;

        message.setTo(jid);
        message.setType(QXmppMessage::Normal);
        message.setMucInvitationJid(m_room->jid());

        if(!m_room->password().isEmpty())
            message.setBody(QString("Password: %1").arg(m_room->password()));

        m_xmppClient->sendPacket(message);
    }

    ui->pushSend->setEnabled(true);
}
