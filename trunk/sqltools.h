#ifndef SQLTOOLS_H
#define SQLTOOLS_H

#include <QStringList>
#include <QString>
#include <QList>
#include <QMap>

class SqlTools
{
public:
    static QList<QStringList> query(const QString &s, const QMap<QString, QString> &binds = QMap<QString, QString>());
};

#endif // SQLTOOLS_H
