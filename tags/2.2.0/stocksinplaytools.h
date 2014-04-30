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

#ifndef STOCKSINPLAYTOOLS_H
#define STOCKSINPLAYTOOLS_H

#include <QObject>
#include <QString>

class StocksInPlayTools
{
public:
    static QString addFromStocksInPlayTitle();
    static QString stocksInPlayTitle();

    static QString cachedHash();
    static void setCachedHash(const QString &h);

private:
    StocksInPlayTools();

    static QString m_cachedHash;
};

inline
QString StocksInPlayTools::addFromStocksInPlayTitle()
{
    //: This is the label on a menu item that user clicks to issue the command. Stocks In Play - Stock Screener, http://stocksinplay.ru. It's ok not to translate "Stocks In Play" (e.g. you can just copy-paste "Stocks In Play" to your translation)
    return QObject::tr("Add from Stocks In Play");
}

inline
QString StocksInPlayTools::stocksInPlayTitle()
{
    //: Stocks In Play - Stock Screener, http://stocksinplay.ru. It's ok not to translate "Stocks In Play" (e.g. you can just copy-paste "Stocks In Play" to your translation)
    return QObject::tr("Stocks In Play");
}

inline
QString StocksInPlayTools::cachedHash()
{
    return StocksInPlayTools::m_cachedHash;
}

inline
void StocksInPlayTools::setCachedHash(const QString &h)
{
    StocksInPlayTools::m_cachedHash = h;
}

#endif // STOCKSINPLAYTOOLS_H
