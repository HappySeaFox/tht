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

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "settings.h"
#include "sqltools.h"

QList<QVariantList> SqlTools::query(const QString &s, const QString &bindTemplate, const QString &bindValue)
{
    QMap<QString, QString> binds;

    binds.insert(bindTemplate, bindValue);

    return SqlTools::query(s, binds);
}

QList<QVariantList> SqlTools::query(const QString &s, const QMap<QString, QString> &binds)
{
    // databases to query
    QList<QSqlDatabase> databases;

    const QDateTime p_ts = Settings::instance()->persistentDatabaseTimestamp();
    const QDateTime m_ts = Settings::instance()->mutableDatabaseTimestamp();

    // set database priorities
    if(p_ts.isValid() && (!m_ts.isValid() || p_ts > m_ts))
    {
        databases.append(QSqlDatabase::database(Settings::instance()->persistentDatabaseName()));
        databases.append(QSqlDatabase::database(Settings::instance()->mutableDatabaseName()));
    }
    else
    {
        databases.append(QSqlDatabase::database(Settings::instance()->mutableDatabaseName()));
        databases.append(QSqlDatabase::database(Settings::instance()->persistentDatabaseName()));
    }

    QMap<QString, QString>::const_iterator itEnd = binds.end();
    QList<QVariantList> result;
    QSqlQuery query;

    foreach(QSqlDatabase db, databases)
    {
        if(!db.isOpen())
            continue;

        query = QSqlQuery(db);

        if(!query.prepare(s))
        {
            qDebug("Cannot prepare query for '%s' database", qPrintable(db.connectionName()));
            continue;
        }

        for(QMap<QString, QString>::const_iterator it = binds.begin();it != itEnd;++it)
            query.bindValue(it.key(), it.value());

        if(!query.exec())
        {
            qDebug("Cannot execute query for '%s' database", qPrintable(db.connectionName()));
            continue;
        }

        // found something
        if(query.next())
        {
            qDebug("Found data in '%s' database", qPrintable(db.connectionName()));
            break;
        }
    }

    if(!query.isValid())
        return result;

    do
    {
        int index = 0;
        QVariant var;
        QVariantList list;

        while((var = query.value(index++)).isValid())
            list.append(var);

        result.append(list);
    }while(query.next());

    return result;
}

QStringList SqlTools::sectors()
{
    static QStringList s = SqlTools::sectorsReal();
    return s;
}

QStringList SqlTools::industries()
{
    static QStringList i = SqlTools::industriesReal();
    return i;
}

QMap<QString, QStringList> SqlTools::sectorsAndIndustries()
{
    static QMap<QString, QStringList> r = SqlTools::sectorsAndIndustriesReal();
    return r;
}

QStringList SqlTools::sectorsReal()
{
    QStringList result;

    QList<QVariantList> lists = SqlTools::query("SELECT DISTINCT sector FROM tickers");

    foreach(QVariantList l, lists)
    {
        if(l.isEmpty())
            continue;

        result.append(l.at(0).toString());
    }

    return result;
}

QStringList SqlTools::industriesReal()
{
    QStringList result;

    QList<QVariantList> lists = SqlTools::query("SELECT DISTINCT industry FROM tickers");

    foreach(QVariantList l, lists)
    {
        if(l.isEmpty())
            continue;

        result.append(l.at(0).toString());
    }

    return result;
}

QMap<QString, QStringList> SqlTools::sectorsAndIndustriesReal()
{
    QMap<QString, QStringList> result;

    QStringList sectors = SqlTools::sectors();

    foreach(QString sector, sectors)
    {
        QStringList industries;

        QList<QVariantList> lists = SqlTools::query("SELECT DISTINCT industry FROM tickers WHERE sector = :sector",
                                                   ":sector", sector);

        foreach(QVariantList l, lists)
        {
            if(l.isEmpty())
                continue;

            industries.append(l.at(0).toString());
        }

        result.insert(sector, industries);
    }

    return result;
}
