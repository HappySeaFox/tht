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
QIcon ChatTools::m_chatIcon;
QIcon ChatTools::m_unreadIcon;
QHash<QXmppPresence::AvailableStatusType, QIcon> ChatTools::m_statusIcons;
QString ChatTools::m_companyInfoTemplate;

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

    ChatTools::m_chatIcon = QIcon(":/images/chat.ico");
    ChatTools::m_unreadIcon = QIcon(":/images/unread.png");

    ChatTools::m_rxLink = QRegExp("((([A-Za-z]{3,9}:(?:\\/\\/)?)(?:[\\-;:&=\\+\\$,\\w]+@)?[A-Za-z0-9\\.\\-]+|(?:[wW]{3}\\.|[\\-;:&=\\+\\$,\\w]+@)[A-Za-z0-9\\.\\-]+)((?:\\/[\\+~%\\/\\.\\w\\-]*)?\\0077?(?:[\\-\\+=&;%@\\.\\w]*)#?(?:[\\.\\!\\/\\\\\\w]*))?)");

    ChatTools::m_statusIcons.insert(QXmppPresence::Online,    QIcon(":/images/status-available.png"));
    ChatTools::m_statusIcons.insert(QXmppPresence::Chat,      QIcon(":/images/status-available.png"));
    ChatTools::m_statusIcons.insert(QXmppPresence::Away,      QIcon(":/images/status-idle.png"));
    ChatTools::m_statusIcons.insert(QXmppPresence::XA,        QIcon(":/images/status-away.png"));
    ChatTools::m_statusIcons.insert(QXmppPresence::DND,       QIcon(":/images/status-offline.png"));
    ChatTools::m_statusIcons.insert(QXmppPresence::Invisible, QIcon(":/images/status-invisible.png"));

    ChatTools::m_companyInfoTemplate =
                QString("<br><table>")
                + "<tr><td>" + QObject::tr("Company:")        + "</td><td>%1</td></tr>"
                //: Noun. Means "Market exchange:", http://www.investopedia.com/terms/e/exchange.asp
                + "<tr><td>" + QObject::tr("Exchange:")       + "</td><td>%2</td></tr>"
                + "<tr><td>" + QObject::tr("Sector:")         + "</td><td>%3</td></tr>"
                + "<tr><td>" + QObject::tr("Industry:")       + "</td><td>%4</td></tr>"
                //: Means "Company capitalization" as in finance industry
                + "<tr><td>" + QObject::tr("Capitalization:") + "</td><td>%L5 %6</td></tr>"
                + "</table><br>";

    qsrand(time(0) + QCoreApplication::applicationPid());
}

QString ChatTools::cssForLinks()
{
    static QString css = ChatTools::cssForLinksReal();
    return css;
}

QColor ChatTools::randomColor()
{
    if(ChatTools::m_tempColors.isEmpty())
        ChatTools::m_tempColors = ChatTools::m_colors;

    return ChatTools::m_tempColors.takeAt(qrand() % ChatTools::m_tempColors.size());
}

QString ChatTools::escapeBrackets(const QString &plain)
{
    QString rich;

    rich.reserve(int(plain.length() * qreal(1.1)));

    for(int i = 0; i < plain.length(); ++i)
    {
        if (plain.at(i) == '<')
            rich +="&lt;";
        else if (plain.at(i) == '>')
            rich += "&gt;";
        else
            rich += plain.at(i);
    }

    return rich;
}

ChatTools::ChatTools()
{}

QString ChatTools::cssForLinksReal()
{
    QList<QColor> colors = ChatTools::colors();
    QString css;

    css.reserve(28 * colors.size());

    foreach(QColor c, colors)
    {
        QString name = c.name();
        css += QString("a.%1 {color: %2;}\n").arg(QString(name).replace(0, 1, 'c')).arg(name);
    }

    css.squeeze();

    return css;
}

QString ChatTools::companyInfoTemplate()
{
    return ChatTools::m_companyInfoTemplate;
}
