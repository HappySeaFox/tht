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
