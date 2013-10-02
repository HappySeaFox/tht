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

    //: This is the label on a menu item that user clicks to issue the command. It's ok not to translate "Briefing Stock Splits" (e.g. you can just copy-paste "Briefing Stock Splits" to your translation). See http://www.briefing.com/investor/calendars/stock-splits
    QMenu *menu = new QMenu(tr("Add from Briefing Stock Splits"));

    if(!menu)
        return false;

    menu->setIcon(QIcon(":/images/briefing.png"));
    menu->clear();

    QDate currentDate = QDate::currentDate();
    QList<QDate> dates = QList<QDate>() << currentDate << currentDate.addMonths(1) << currentDate.addMonths(2);

    foreach(QDate d, dates)
    {
        QAction *a = menu->addAction(QDate::longMonthName(d.month(), QDate::StandaloneFormat),
                                        this,
                                        SLOT(slotAdd()));

        a->setData(QString("http://www.briefing.com/investor/calendars/stock-splits/%1/%2")
                   .arg(d.year())
                   .arg(d.month()));
    }

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
