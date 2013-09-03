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
#include <QMessageBox>
#include <QTimer>

#include "sendinvitations.h"
#include "chatsettings.h"
#include "ui_sendinvitations.h"

#include "QXmppMucManager.h"
#include "QXmppMessage.h"
#include "QXmppClient.h"

#include "settings.h"

SendInvitations::SendInvitations(QXmppMucRoom *room, QXmppClient *client, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendInvitations),
    m_room(room),
    m_xmppClient(client)
{
    ui->setupUi(this);

    if(!SETTINGS_GET_BOOL(SETTING_CHAT_INVITATIONS_NOTE_SEEN))
        QTimer::singleShot(0, this, SLOT(slotHelpMessage()));
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

        if(!m_xmppClient->sendPacket(message))
        {
            QMessageBox::critical(this, tr("Error"), tr("Error sending invitation to %1").arg(jid));
            break;
        }
    }

    ui->pushSend->setEnabled(true);
}

void SendInvitations::slotHelpMessage()
{
    SETTINGS_SET_BOOL(SETTING_CHAT_INVITATIONS_NOTE_SEEN, true);
    QMessageBox::information(this, tr("Information"), tr("Please remember that the server can decline your invitations depending on its configuration"));
}
