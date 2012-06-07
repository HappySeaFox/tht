#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "settings.h"
#include "sqltools.h"

QList<QStringList> SqlTools::query(const QString &s, const QString &bindTemplate, const QString &bindValue)
{
    QMap<QString, QString> binds;

    binds.insert(bindTemplate, bindValue);

    return SqlTools::query(s, binds);
}

QList<QStringList> SqlTools::query(const QString &s, const QMap<QString, QString> &binds)
{
    QSqlDatabase dbp = QSqlDatabase::database(Settings::instance()->tickersPersistentDatabaseName());
    QSqlDatabase dbm = QSqlDatabase::database(Settings::instance()->tickersMutableDatabaseName());
    QList<QStringList> result;

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
        QStringList list;

        while((var = query.value(index++)).isValid())
            list.append(var.toString());

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

    QList<QStringList> lists = SqlTools::query("SELECT DISTINCT sector FROM tickers");

    foreach(QStringList l, lists)
    {
        if(l.isEmpty())
            continue;

        result.append(l.at(0));
    }

    return result;
}

QStringList SqlTools::industriesReal()
{
    QStringList result;

    QList<QStringList> lists = SqlTools::query("SELECT DISTINCT industry FROM tickers");

    foreach(QStringList l, lists)
    {
        if(l.isEmpty())
            continue;

        result.append(l.at(0));
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

        QList<QStringList> lists = SqlTools::query("SELECT DISTINCT industry FROM tickers WHERE sector = :sector",
                                                   ":sector", sector);

        foreach(QStringList l, lists)
        {
            if(l.isEmpty())
                continue;

            industries.append(l.at(0));
        }

        result.insert(sector, industries);
    }

    return result;
}
