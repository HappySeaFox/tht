/*
 * This file is part of THT-lib.
 *
 * THT-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation.
 *
 * THT-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SQLTOOLS_H
#define SQLTOOLS_H

#include <QStringList>
#include <QVariant>
#include <QString>
#include <QList>
#include <QMap>

/*
 *  Tools to query SQLite database with tickers
 *  bundled with THT
 */
class SqlTools
{
public:
    /*
     *  Query the database
     */
    static QList<QVariantList> query(const QString &s, const QString &bindTemplate, const QString &bindValue);
    static QList<QVariantList> query(const QString &s, const QMap<QString, QString> &binds = QMap<QString, QString>());

    /*
     *  Available sectors, industries and exchanges
     */
    static QStringList sectors();
    static QStringList industries();
    static QStringList exchanges();

    static QMap<QString, QStringList> sectorsAndIndustries();

private:
    SqlTools();

    static QStringList selectDistinct(const QString &query);

    static QMap<QString, QStringList> sectorsAndIndustriesReal();

    static void initializeDatabases();
};

#endif // SQLTOOLS_H
