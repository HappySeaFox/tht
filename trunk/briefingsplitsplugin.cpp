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

#include <QAction>
#include <QDate>
#include <QMenu>
#include <QUrl>

#include "briefingsplitsdownloader.h"
#include "briefingsplitsplugin.h"

BriefingSplitsPlugin::BriefingSplitsPlugin() :
    PluginImportExport()
{
    THT_PLUGIN_INTERFACE_IMPLEMENTATION
}

bool BriefingSplitsPlugin::embed(int list, QMenu *parentMenu)
{
    PluginImportExport::Embeds &emb = embeds();

    if(emb.contains(list))
        return true;

    QMenu *menu = new QMenu(tr("Add from Briefing Stock Splits"));

    if(!menu)
        return false;

    menu->setIcon(QIcon(":/images/briefing.png"));
    menu->clear();

    QAction *a;
    QDate currentDate = QDate::currentDate();
    QDate nextMonthDate = currentDate.addMonths(1);

    a = menu->addAction(tr("This month"), this, SLOT(slotAdd()));
    a->setData(QString("http://www.briefing.com/investor/calendars/stock-splits/%1/%2")
               .arg(currentDate.year())
               .arg(currentDate.month()));

    a = menu->addAction(tr("Next month"), this, SLOT(slotAdd()));
    a->setData(QString("http://www.briefing.com/investor/calendars/stock-splits/%1/%2")
               .arg(nextMonthDate.year())
               .arg(nextMonthDate.month()));

    parentMenu->addMenu(menu);

    emb.insert(list, menu);

    return true;
}

void BriefingSplitsPlugin::addFromBriefingSplits(int list, const QUrl &u)
{
    BriefingSplitsDownloader dn(u, topLevelWidget());

    if(dn.exec() != QDialog::Accepted)
        return;

    emit sendTickers(list, dn.tickers());
}

void BriefingSplitsPlugin::slotAdd()
{
    qDebug("Add from BriefingSplits");

    QAction *a = qobject_cast<QAction *>(sender());
    int list = senderMenuActionToList();

    if(!a || list < 0)
        return;

    QUrl u = a->data().toUrl();

    if(!u.isValid())
    {
        qDebug("Url \"%s\" is not valid", qPrintable(u.toString(QUrl::RemovePassword)));
        return;
    }

    addFromBriefingSplits(list, u);
}

PLUGIN_CONSTRUCTOR(BriefingSplitsPlugin)
