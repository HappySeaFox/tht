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

#ifndef FINVIZURL_H
#define FINVIZURL_H

#include <QMetaType>
#include <QString>
#include <QList>
#include <QUrl>

class QDataStream;

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

// serialize/deserialize FinvizUrl
QDataStream &operator<<(QDataStream &out, const FinvizUrl &fu);
QDataStream &operator>>(QDataStream &in, FinvizUrl &fu);

#define SETTINGS_GET_FINVIZ_URLS_OLD Settings::instance()->value<QList<FinvizUrl> >
#define SETTINGS_GET_FINVIZ_URLS Settings::instance()->binaryValue<QList<FinvizUrl> >
#define SETTINGS_SET_FINVIZ_URLS Settings::instance()->setBinaryValue<QList<FinvizUrl> >

#define SETTING_FINVIZ_EMAIL        "finviz-email"
#define SETTING_FINVIZ_PASSWORD_152 "finviz-password"
#define SETTING_FINVIZ_PASSWORD     "finviz-password2"
#define SETTING_FINVIZ_URLS         "finviz-urls"

#endif // FINVIZURL_H
