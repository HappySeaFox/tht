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

#include "ticker.h"

class ListItem : public QListWidgetItem
{
public:
    explicit ListItem(QListWidget *parent = 0);
    explicit ListItem(const QString &text, QListWidget *parent = 0);
    ~ListItem();

    Ticker::Priority priority() const;
    void setPriority(Ticker::Priority, bool force = false);

    QString comment() const;
    void setComment(const QString &c);

    void resetPriority();

    virtual bool operator<(const QListWidgetItem &other) const;

private:
    Ticker::Priority m_priority;
    QString m_comment;
};

inline
Ticker::Priority ListItem::priority() const
{
    return m_priority;
}

inline
QString ListItem::comment() const
{
    return m_comment;
}

inline
void ListItem::setComment(const QString &c)
{
    m_comment = c;
}

inline
void ListItem::resetPriority()
{
    setPriority(Ticker::PriorityNormal, true);
}

#endif // LISTITEM_H
