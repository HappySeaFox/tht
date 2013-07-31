/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>

#include "settings.h"
#include "sqltools.h"

class SqlToolsPrivate
{
public:
    static bool initialized;
};

bool SqlToolsPrivate::initialized = false;

/*****************************************/

QList<QVariantList> SqlTools::query(const QString &s, const QString &bindTemplate, const QString &bindValue)
{
    QMap<QString, QString> binds;

    binds.insert(bindTemplate, bindValue);

    return SqlTools::query(s, binds);
}

QList<QVariantList> SqlTools::query(const QString &s, const QMap<QString, QString> &binds)
{
    if(!SqlToolsPrivate::initialized)
        SqlTools::initializeDatabases();

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
    static QStringList s = SqlTools::selectDistinct("SELECT DISTINCT sector FROM tickers");
    return s;
}

QStringList SqlTools::industries()
{
    static QStringList i = SqlTools::selectDistinct("SELECT DISTINCT industry FROM tickers");
    return i;
}

QStringList SqlTools::exchanges()
{
    static QStringList i = SqlTools::selectDistinct("SELECT DISTINCT exchange FROM tickers");
    return i;
}

QMap<QString, QStringList> SqlTools::sectorsAndIndustries()
{
    static QMap<QString, QStringList> r = SqlTools::sectorsAndIndustriesReal();
    return r;
}

SqlTools::SqlTools()
{}

QStringList SqlTools::selectDistinct(const QString &query)
{
    QStringList result;

    QList<QVariantList> lists = SqlTools::query(query);

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

void SqlTools::initializeDatabases()
{
    SqlToolsPrivate::initialized = true;

    qDebug("Initializing ticker databases");

    // open ticker databases
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", Settings::instance()->mutableDatabaseName());
    db.setDatabaseName(Settings::instance()->mutableDatabasePath());

    if(!QFile::exists(db.databaseName()) || !db.isValid() || !db.open())
        qDebug("Cannot open mutable database (%s)", qPrintable(db.lastError().text()));
    else
        qDebug("Mutable database has been opened");

    db = QSqlDatabase::addDatabase("QSQLITE", Settings::instance()->persistentDatabaseName());
    db.setDatabaseName(Settings::instance()->persistentDatabasePath());

    if(!QFile::exists(db.databaseName()) || !db.isValid() || !db.open())
        qDebug("Cannot open persistent database (%s)", qPrintable(db.lastError().text()));
    else
        qDebug("Persistent database has been opened");
}
