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

#include <QTreeWidgetItem>
#include <QHeaderView>

#include "pluginmanager.h"
#include "plugindetails.h"
#include "pluginloader.h"
#include "ui_pluginmanager.h"

PluginManager::PluginManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginManager)
{
    ui->setupUi(this);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->treePlugins->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->treePlugins->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    QList<Plugin::Type> types = QList<Plugin::Type>()
                                    << Plugin::Common
                                    << Plugin::AddTickersFrom
                                    << Plugin::ExportTickersTo;

    foreach(Plugin::Type type, types)
    {
        QList<Plugin *> plugins = PluginLoader::instance()->byType(type);

        if(plugins.isEmpty())
            continue;

        QTreeWidgetItem *i = new QTreeWidgetItem(ui->treePlugins, QStringList() << typeToString(type));
        ui->treePlugins->addTopLevelItem(i);
        ui->treePlugins->expandItem(i);

        foreach(Plugin *p, plugins)
        {
            QTreeWidgetItem *j = new QTreeWidgetItem(QStringList()
                                                            << p->property(THT_PLUGIN_PROPERTY_NAME).toString()
                                                            << p->property(THT_PLUGIN_PROPERTY_VERSION).toString()
                                                            << p->property(THT_PLUGIN_PROPERTY_AUTHOR).toString()
                                                        );

            j->setData(0, Qt::UserRole, p->property(THT_PLUGIN_PROPERTY_UUID).toString());
            i->addChild(j);
        }
    }
}

PluginManager::~PluginManager()
{
    delete ui;
}

QString PluginManager::typeToString(int type)
{
    switch(type)
    {
        case Plugin::Common:
        //: Means "Common plugins" (in the plural)
        return tr("Common");

        case Plugin::AddTickersFrom:
        return tr("Add tickers");

        case Plugin::ExportTickersTo:
        return tr("Export tickers");

        default:
        //: Means "Unknown plugins" (in the plural)
        return tr("Unknown");
    }
}

void PluginManager::slotCurrentItemChanged(QTreeWidgetItem *current)
{
    ui->pushDetails->setDisabled(current->childCount());
}

void PluginManager::slotDetails()
{
    QTreeWidgetItem *i = ui->treePlugins->currentItem();

    if(!i || i->childCount())
        return;

    PluginDetails pd(i->data(0, Qt::UserRole).toString(), this);
    pd.exec();
}
