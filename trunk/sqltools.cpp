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
        query = QSqlQuery(dbp);
        query.prepare(s);

        for(QMap<QString, QString>::const_iterator it = binds.begin();it != itEnd;++it)
            query.bindValue(it.key(), it.value());

        query.exec();
        query.next();
    }

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
