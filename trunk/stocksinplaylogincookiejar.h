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

#ifndef STOCKSINPLAYLOGINCOOKIEJAR_H
#define STOCKSINPLAYLOGINCOOKIEJAR_H

#include <QNetworkCookieJar>
#include <QByteArray>
#include <QString>

class StocksInPlayLoginCookieJar : public QNetworkCookieJar
{
public:
    StocksInPlayLoginCookieJar(QObject *parent = 0);

    QString id() const;
    QString hash() const;

private:
    QByteArray stringValue(const QString &name) const;
};

inline
QString StocksInPlayLoginCookieJar::id() const
{
    return stringValue("id");
}

inline
QString StocksInPlayLoginCookieJar::hash() const
{
    return stringValue("hash");
}

#endif // STOCKSINPLAYLOGINCOOKIEJAR_H
