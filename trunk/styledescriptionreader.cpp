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

#include <QSettings>
#include <QFile>

#include "styledescriptionreader.h"

StyleDescriptionReader::StyleDescriptionReader()
{
}

StyleDescriptionReader::~StyleDescriptionReader()
{}

bool StyleDescriptionReader::parse(const QString &filePath)
{
    if(!QFile::exists(filePath))
        return false;

    QSettings settings(filePath, QSettings::IniFormat);

    m_name = settings.value("name").toString();
    m_previewColor = QColor(settings.value("preview-color").toString());

    return true;
}
