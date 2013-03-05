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

#include <QTimer>

#include "tickerinformationfetcher.h"
#include "sqltools.h"

TickerInformationFetcher::TickerInformationFetcher(QObject *parent) :
    QObject(parent)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(0);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotFetch()));
}

void TickerInformationFetcher::fetch(const QString &ticker)
{
    m_ticker = ticker;
    m_timer->start();
}

void TickerInformationFetcher::slotFetch()
{
    QList<QVariantList> lists = SqlTools::query("SELECT company, exchange, sector, industry FROM tickers WHERE ticker = :ticker", ":ticker", m_ticker);

    if(!lists.isEmpty())
    {
        QVariantList values = lists.at(0);

        // must be 4 values
        if(values.size() == 4)
        {
            QString company = values.at(0).toString();

            if(!company.isEmpty())
            {
                emit done(company, values.at(1).toString(), values.at(2).toString(), values.at(3).toString());
                return;
            }
        }
    }

    // error
    emit done();
}
