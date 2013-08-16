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

#include <QCoreApplication>

#include <ctime>

#include "chattools.h"

QList<QColor> ChatTools::m_colors;
QList<QColor> ChatTools::m_tempColors;
QRegExp ChatTools::m_rxLink;
QIcon ChatTools::m_unreadIcon;
QHash<QXmppPresence::AvailableStatusType, QIcon> ChatTools::m_statusIcons;

void ChatTools::init()
{
    if(!ChatTools::m_colors.isEmpty())
        return;

    ChatTools::m_colors << QColor(0,0,0)
                        << QColor(0,0,80)
                        << QColor(128,128,0)
                        << QColor(0,0,128)
                        << QColor(0,128,0)
                        << QColor(128,0,128)
                        << QColor(128,0,0)
                        << QColor(90,90,90)
                        << QColor(41,133,199)
                        << QColor(0,85,175)
                        << QColor(0,170,0)
                        << QColor(210,0,0)
                        << QColor(205,80,80)
                           ;

    ChatTools::m_unreadIcon = QIcon(":/images/unread.png");

    ChatTools::m_rxLink = QRegExp("((([A-Za-z]{3,9}:(?:\\/\\/)?)(?:[\\-;:&=\\+\\$,\\w]+@)?[A-Za-z0-9\\.\\-]+|(?:www\\.|[\\-;:&=\\+\\$,\\w]+@)[A-Za-z0-9\\.\\-]+)((?:\\/[\\+~%\\/\\.\\w\\-]*)?\\0077?(?:[\\-\\+=&;%@\\.\\w]*)#?(?:[\\.\\!\\/\\\\\\w]*))?)");

    ChatTools::m_statusIcons.insert(QXmppPresence::Online,    QIcon(":/images/status-available.png"));
    ChatTools::m_statusIcons.insert(QXmppPresence::Chat,      QIcon(":/images/status-available.png"));
    ChatTools::m_statusIcons.insert(QXmppPresence::Away,      QIcon(":/images/status-idle.png"));
    ChatTools::m_statusIcons.insert(QXmppPresence::XA,        QIcon(":/images/status-away.png"));
    ChatTools::m_statusIcons.insert(QXmppPresence::DND,       QIcon(":/images/status-offline.png"));
    ChatTools::m_statusIcons.insert(QXmppPresence::Invisible, QIcon(":/images/status-invisible.png"));

    qsrand(time(0) + QCoreApplication::applicationPid());
}

QString ChatTools::cssForLinks()
{
    QList<QColor> colors = ChatTools::colors();
    QString css;

    foreach(QColor c, colors)
    {
        QString name = c.name();
        css += QString("a.%1 {color: %2;}\n").arg(QString(name).replace(0, 1, 'c')).arg(name);
    }

    return css;
}

QColor ChatTools::randomColor()
{
    if(ChatTools::m_tempColors.isEmpty())
        ChatTools::m_tempColors = ChatTools::m_colors;

    return ChatTools::m_tempColors.takeAt(qrand() % ChatTools::m_tempColors.size());
}

QRegExp ChatTools::urlRegExp()
{
    return ChatTools::m_rxLink;
}

QIcon ChatTools::unreadIcon()
{
    return ChatTools::m_unreadIcon;
}

QIcon ChatTools::statusIcon(QXmppPresence::AvailableStatusType type)
{
    return ChatTools::m_statusIcons.value(type);
}

ChatTools::ChatTools()
{}
