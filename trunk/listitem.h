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

#ifndef LISTITEM_H
#define LISTITEM_H

#include <QListWidgetItem>
#include <QString>

class ListItem : public QListWidgetItem
{
public:
    explicit ListItem(QListWidget *parent = 0);
    explicit ListItem(const QString &text, QListWidget *parent = 0);

    enum Priority { PriorityNormal, PriorityHigh, PriorityHighest };

    Priority priority() const;

    void setPriority(Priority, bool force = false);

    static QString priorityToString(Priority);

    void removePriority();
    void resetPriority();

private:
    Priority m_priority;
};

inline
ListItem::Priority ListItem::priority() const
{
    return m_priority;
}

inline
QString ListItem::priorityToString(ListItem::Priority pr)
{
    return QString("priority%1").arg(pr);
}

#endif // LISTITEM_H
