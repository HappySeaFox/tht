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

#include <QList>

#include "linkpointsession.h"
#include "datamanagerbase.h"
#include "linkedwindow.h"

#define SETTINGS_GET_LINKED_WINDOWS Settings::instance()->value<QList<LinkedWindow> >
#define SETTINGS_SET_LINKED_WINDOWS Settings::instance()->setValue<QList<LinkedWindow> >

#define SETTINGS_GET_LINKS Settings::instance()->value<QList<LinkPointSession> >
#define SETTINGS_SET_LINKS Settings::instance()->setValue<QList<LinkPointSession> >

#define SETTING_LAST_LINKS "last-links"
#define SETTING_LINKS      "links"

class LinkPointManager : public DataManagerBase
{
    Q_OBJECT

public:
    explicit LinkPointManager(const LinkPointSession &currentLinks, QWidget *parent = 0);
    ~LinkPointManager();

    QList<LinkPointSession> links() const;

private:
    void addLinkPointSession(const LinkPointSession &, bool edit = false);

private slots:
    virtual void slotAdd();

private:
    LinkPointSession m_currentLinks;
};

#endif // LINKPOINTMANAGER_H
