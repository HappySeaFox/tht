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

#ifndef LINKPOINTMANAGER_H
#define LINKPOINTMANAGER_H

#include <QPoint>
#include <QList>

#include "datamanagerbase.h"
#include "settings.h"

class LinkPointManager : public DataManagerBase
{
    Q_OBJECT

public:
    explicit LinkPointManager(const QList<QPoint> &currentLinks, QWidget *parent = 0);
    ~LinkPointManager();

    QList<LinkPoint> links() const;

private:
    void addLinkPoint(const LinkPoint &, bool edit = false);

private slots:
    virtual void slotAdd();

private:
    QList<QPoint> m_currentLinks;
};

#endif // LINKPOINTMANAGER_H
