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
    QSqlDatabase dbp = QSqlDatabase::database(Settings::instance()->persistentDatabaseName());
    QSqlDatabase dbm = QSqlDatabase::database(Settings::instance()->mutableDatabaseName());
    QList<QVariantList> result;

    QSqlQuery query(dbm);

    QMap<QString, QString>::const_iterator itEnd = binds.end();

    query.prepare(s);

    for(QMap<QString, QString>::const_iterator it = binds.begin();it != itEnd;++it)
        query.bindValue(it.key(), it.value());

    query.exec();

    if(!query.next())
    {
        qDebug("Querying mutable database");

        query = QSqlQuery(dbp);
        query.prepare(s);

        for(QMap<QString, QString>::const_iterator it = binds.begin();it != itEnd;++it)
            query.bindValue(it.key(), it.value());

        query.exec();
        query.next();
    }
    else
        qDebug("Querying persistent database");

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
