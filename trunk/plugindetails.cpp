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

#include "plugindetails.h"
#include "pluginloader.h"
#include "ui_plugindetails.h"

PluginDetails::PluginDetails(const QString &uuid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginDetails)
{
    ui->setupUi(this);

    Plugin *plugin = PluginLoader::instance()->byUuid(uuid);

    if(plugin)
    {
        ui->labelFilename->setText(plugin->property(THT_PLUGIN_PROPERTY_FILENAME).toString());
        ui->labelName->setText(plugin->property(THT_PLUGIN_PROPERTY_NAME).toString());
        ui->labelAuthor->setText(plugin->property(THT_PLUGIN_PROPERTY_AUTHOR).toString());
        ui->labelVersion->setText(plugin->property(THT_PLUGIN_PROPERTY_VERSION).toString());
        ui->labelUrl->setText(plugin->property(THT_PLUGIN_PROPERTY_URL).toString());
        ui->labelUuid->setText(plugin->property(THT_PLUGIN_PROPERTY_UUID).toString());
    }
}

PluginDetails::~PluginDetails()
{
    delete ui;
}
