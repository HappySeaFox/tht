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

#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QRegExp>
#include <QUrl>

class NetworkAccess;
class QNetworkReply;

class UpdateChecker : public QObject
{
    Q_OBJECT

public:
    explicit UpdateChecker(QObject *parent = 0);

    void start();

signals:
    void newVersion(const QString &); // empty if no updates
    void error(const QString &);

private slots:
    void slotFinished();

private:
    explicit UpdateChecker();

    NetworkAccess *m_net;
    QString m_lastVersion;
    QRegExp m_rxVersion;
    QUrl m_url;
};

#endif // UPDATECHECKER_H
