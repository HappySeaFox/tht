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

#include <QStringList>
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QList>
#include <QDate>
#include <QMap>

class IdleTimer;
class NetworkAccess;

struct TickerForDb
{
    TickerForDb()
        : cap(0)
    {}

    QString ticker;
    QString company;
    QString sector;
    QString industry;
    QString exchange;
    QString country;

    double cap;
};

inline bool operator<(const TickerForDb &a, const TickerForDb &b)
{
    return a.ticker < b.ticker;
}

class TickersDatabaseUpdater : public QObject
{
    Q_OBJECT

public:
    explicit TickersDatabaseUpdater(QObject *parent = 0);

private:
    void proceedToTickers();
    bool addFomcDates(const QString &data, const QString &title);
    bool nextExchange();

private slots:
    void slotFinishedFomc();
    void slotFinished();
    void slotStartRequest();
    void slotParseAnswer();

private:
    NetworkAccess *m_net;
    QList<QDate> m_fomcDates;
    QString m_newDb, m_newTs;
    QStringList m_exchanges;
    IdleTimer *m_idleTimer;
    QMap<QString, QByteArray> m_data;
};

#endif // TICKERSDATABASEUPDATER_H
