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

#include <QToolButton>
#include <QMetaType>
#include <QLayout>
#include <QHash>
#include <QIcon>

#include "chatsettings.h"
#include "chatwindow.h"
#include "chatplugin.h"
#include "settings.h"
#include "roominfo.h"
#include "tools.h"

ChatPlugin::ChatPlugin() :
    Plugin(),
    m_window(0)
{
    THT_PLUGIN_INTERFACE_IMPLEMENTATION

    qRegisterMetaTypeStreamOperators<RoomInfo>("RoomInfo");
    qRegisterMetaTypeStreamOperators<QList<RoomInfo> >("QList<RoomInfo>");

    QHash<QString, QVariant> defaltValues;

    defaltValues.insert(SETTING_CHAT_POSITION, Tools::invalidQPoint);

    Settings::instance()->addDefaultValues(defaltValues);
}

ChatPlugin::~ChatPlugin()
{
    delete m_window;
}

bool ChatPlugin::init()
{
    m_button = new QToolButton;

    if(!m_button)
    {
        qWarning("Cannot create QPushButton object");
        return false;
    }

    m_button->setFixedSize(22, 22);
    m_button->setIcon(QIcon(":/images/chat.png"));
    connect(m_button, SIGNAL(clicked()), this, SLOT(slotClicked()));

    return true;
}

void ChatPlugin::delayedInit()
{
    QWidget *c = containerLeft();

    if(!c)
        return;

    m_button->setParent(c);
    c->layout()->addWidget(m_button);
}

void ChatPlugin::slotClicked()
{
    if(!m_window)
    {
        m_window = new ChatWindow;
        m_window->setWindowIcon(QIcon(":/images/chat.png"));
        connect(m_window, SIGNAL(openTicker(QString)), this, SIGNAL(openTicker(QString)));
    }

    m_window->show();
}

PLUGIN_CONSTRUCTOR(ChatPlugin)
