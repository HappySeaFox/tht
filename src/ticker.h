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

#ifndef TICKER_H
#define TICKER_H

#include <QString>

struct Ticker
{
    enum Priority { PriorityNormal, PriorityMedium, PriorityHigh, PriorityHighest };

    Ticker(const QString &_ticker = QString(),
           Priority _priority = PriorityNormal,
           const QString &_comment = QString())
        : ticker(_ticker),
          priority(_priority),
          comment(_comment)
    {}

    QString ticker;
    Priority priority;
    QString comment;

    static QString priorityToString(Ticker::Priority);
};

inline
QString Ticker::priorityToString(Ticker::Priority pr)
{
    return QString("priority%1").arg(pr);
}

#endif // TICKER_H
