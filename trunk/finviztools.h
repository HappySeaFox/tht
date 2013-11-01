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

#ifndef FINVIZTOOLS_H
#define FINVIZTOOLS_H

#include <QObject>
#include <QString>

class FinvizTools
{
public:
    static QString addFromFinvizTitle();

    static QString cachedPassword();
    static void setCachedPassword(const QString &p);

private:
    FinvizTools();

    static QString m_cachedPassword;
};

inline
QString FinvizTools::addFromFinvizTitle()
{
    //: This is the label on a menu item that user clicks to issue the command. Finviz - Stock Screener, http://finviz.com. It's ok not to translate "Finviz" (e.g. you can just copy-paste "Finviz" to your translation)
    return QObject::tr("Add from Finviz");
}

inline
QString FinvizTools::cachedPassword()
{
    return FinvizTools::m_cachedPassword;
}

inline
void FinvizTools::setCachedPassword(const QString &p)
{
    FinvizTools::m_cachedPassword = p;
}

#endif // FINVIZTOOLS_H
