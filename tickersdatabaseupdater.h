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

#ifndef TICKERSDATABASEUPDATER_H
#define TICKERSDATABASEUPDATER_H

#include <QObject>
#include <QString>

class QByteArray;
class NetworkAccess;

class TickersDatabaseUpdater : public QObject
{
    Q_OBJECT

public:
    explicit TickersDatabaseUpdater(QObject *parent = 0);

private:
    void checkNewData();
    bool writeData(const QString &file, const QByteArray &data);

private slots:
    void startRequest();
    void slotFinished();

private:
    NetworkAccess *m_net;
    QString m_baseurl;
    bool m_downloadingData;
};

#endif // TICKERSDATABASEUPDATER_H
