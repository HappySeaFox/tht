/*
 * This file is part of THT-lib.
 *
 * THT-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation.
 *
 * THT-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TICKERINFORMATIONFETCHER_H
#define TICKERINFORMATIONFETCHER_H

#include <QObject>
#include <QString>

class QTimer;

class TickerInformationFetcher : public QObject
{
    Q_OBJECT

public:
    explicit TickerInformationFetcher(QObject *parent = 0);

    void fetch(const QString &);

signals:
    // name is empty on error
    void done(const QString &name = QString(),
              const QString &sector = QString(),
              const QString &industry = QString(),
              const QString &exchange = QString());

private slots:
    void slotFetch();

private:
    QString m_ticker;
    QTimer *m_timer;
};

#endif // TICKERINFORMATIONFETCHER_H
