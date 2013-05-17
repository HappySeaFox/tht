#ifndef FINVIZURL_H
#define FINVIZURL_H

#include <QDataStream>
#include <QMetaType>
#include <QString>
#include <QList>
#include <QUrl>

struct FinvizUrl
{
    FinvizUrl(const QString &_name = QString(), const QUrl &_url = QUrl())
        : name(_name),
          url(_url)
    {}

    QString name;
    QUrl url;
};

Q_DECLARE_METATYPE(FinvizUrl)
Q_DECLARE_METATYPE(QList<FinvizUrl>)

#define SETTINGS_GET_FINVIZ_URLS Settings::instance()->value<QList<FinvizUrl> >
#define SETTINGS_SET_FINVIZ_URLS Settings::instance()->setValue<QList<FinvizUrl> >

#define SETTING_FINVIZ_EMAIL    "finviz-email"
#define SETTING_FINVIZ_PASSWORD "finviz-password"
#define SETTING_FINVIZ_URLS     "finviz-urls"

#endif // FINVIZURL_H
