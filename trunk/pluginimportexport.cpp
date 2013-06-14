/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QAction>
#include <QWidget>
#include <QMenu>

#include "pluginimportexport.h"

PluginImportExport::PluginImportExport() :
    Plugin()
{
    d = new PluginImportExportPrivate;
}

PluginImportExport::~PluginImportExport()
{
    Embeds::const_iterator itEnd = d->embeds.end();

    for(Embeds::const_iterator it = d->embeds.begin();it != itEnd;++it)
    {
        delete it.value();
    }

    delete d;
}

int PluginImportExport::senderStandaloneActionToList() const
{
    int list = -1;

    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return list;

    Embeds::const_iterator itEnd = d->embeds.end();

    for(Embeds::const_iterator it = d->embeds.begin();it != itEnd;++it)
    {
        if(it.value() == a)
        {
            list = it.key();
            break;
        }
    }

    return list;
}

int PluginImportExport::senderMenuActionToList() const
{
    int list = -1;

    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return list;

    QMenu *menu = qobject_cast<QMenu *>(a->parent());

    if(!menu)
        return list;

    Embeds::const_iterator itEnd = d->embeds.end();

    for(Embeds::const_iterator it = d->embeds.begin();it != itEnd;++it)
    {
        if(it.value() == menu)
        {
            list = it.key();
            break;
        }
    }

    return list;
}
