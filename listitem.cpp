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

#include <QPixmapCache>
#include <QPixmap>
#include <QIcon>

#include "listitem.h"
#include "settings.h"

ListItem::ListItem(QListWidget *parent) :
    QListWidgetItem(parent),
    m_priority(PriorityNormal)
{
    setPriority(m_priority, true);
}

ListItem::ListItem(const QString &text, QListWidget *parent) :
    QListWidgetItem(text, parent),
    m_priority(PriorityNormal)
{
    setPriority(m_priority, true);
}

void ListItem::setPriority(ListItem::Priority p, bool force)
{
    if(!Settings::instance()->usePriorities())
        return;

    if(p == m_priority && !force)
        return;

    m_priority = p;

    QPixmap pix;

    if(QPixmapCache::find(ListItem::priorityToString(m_priority), &pix))
        setIcon(pix);
    else
        setIcon(QIcon());
}
