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

#ifndef SQLTOOLS_H
#define SQLTOOLS_H

#include <QVariant>
#include <QString>
#include <QList>
#include <QMap>

class SqlTools
{
public:
    static QList<QVariantList> query(const QString &s, const QString &bindTemplate, const QString &bindValue);
    static QList<QVariantList> query(const QString &s, const QMap<QString, QString> &binds = QMap<QString, QString>());

    static QStringList sectors();
    static QStringList industries();
    static QStringList exchanges();

    static QMap<QString, QStringList> sectorsAndIndustries();

private:
    static QStringList selectDistinct(const QString &query);

    static QMap<QString, QStringList> sectorsAndIndustriesReal();
};

#endif // SQLTOOLS_H
