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

    static QMap<QString, QStringList> sectorsAndIndustries();

private:
    static QStringList sectorsReal();
    static QStringList industriesReal();
    static QMap<QString, QStringList> sectorsAndIndustriesReal();
};

#endif // SQLTOOLS_H
