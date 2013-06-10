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

#include <QFile>

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

        QString url = plugin->property(THT_PLUGIN_PROPERTY_URL).toString();

        if(!url.isEmpty())
        {
            const int maxlen = 45;

            ui->labelUrl->setText(QString("<a href=\"%1\">%2</a>")
                                  .arg(url)
                                  .arg((url.length() > maxlen) ? (url.left(maxlen)+"...") : url));
        }

        ui->labelUuid->setText(plugin->property(THT_PLUGIN_PROPERTY_UUID).toString());

        QFile fr(plugin->property(THT_PLUGIN_PROPERTY_LICENSE_FILE).toString());

        if(fr.open(QIODevice::ReadOnly))
            ui->plainLicense->setPlainText(fr.readAll());
    }
}

PluginDetails::~PluginDetails()
{
    delete ui;
}
