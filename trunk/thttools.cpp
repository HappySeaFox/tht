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

#include <QApplication>
#include <QRegExp>
#include <QFile>
#include <QDir>

#include "styledescriptionreader.h"
#include "settings.h"
#include "thttools.h"

bool THTTools::m_isStyleApplied = false;

THTTools::THTTools()
{}

void THTTools::resetStyle(ResetStyleOnErrorType rt)
{
    QString style = SETTINGS_GET_STRING(SETTING_STYLE);

    qDebug("Style \"%s\"", qPrintable(style));

    if(!style.isEmpty())
    {
        const QString dirWithStyles = QDir::fromNativeSeparators(QCoreApplication::applicationDirPath() + QDir::separator()) + "styles";

        QFile file(dirWithStyles
                    + QDir::separator()
                    + style.replace(QRegExp("ini$"), "qss"));

        if(file.open(QIODevice::ReadOnly))
        {
            QString css = file.readAll();

            qApp->setStyleSheet(css
                                .replace("$SD", dirWithStyles)
                                );

            THTTools::m_isStyleApplied = true;
        }
        else
        {
            qWarning("Style \"%s\" is not found, error: %s", qPrintable(style), qPrintable(file.errorString()));

            if(rt == ResetStyleOnError)
            {
                qApp->setStyleSheet(QString());
                THTTools::m_isStyleApplied = false;
            }
        }
    }
    else
    {
        qApp->setStyleSheet(QString());
        THTTools::m_isStyleApplied = false;
    }
}

bool THTTools::isStyleApplied()
{
    return THTTools::m_isStyleApplied;
}

