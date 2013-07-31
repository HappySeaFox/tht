/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *  This CSV reader is grabbed from http://www.prog.org.ru/topic_12337_0.html
 *  It is written by Filonenko Mikhail, and contains no copyright or license
 *  notice
 */

/*
 *  12.06.2013 update: Filonenko Mikhail allowed to license CsvReader
 *  under the terms of LGPL v3+
 */

#include "csvreader.h"

#include <QString>
#include <QChar>

class CsvReaderPrivate
{
public:
    CsvReaderPrivate()
    {
        pos = 0;
        delim = QChar(',');
        encl = QChar('"');
        term = '\n';
        altTerm = "\r\n";
    }

    QString src;
    int pos;
    QChar delim;
    QChar encl;
    QChar term;
    QString altTerm;
};

CsvReader::CsvReader()
{
    d = new CsvReaderPrivate;
}

CsvReader::CsvReader(const QString& source)
{
    d = new CsvReaderPrivate;

    d->src = source;
}

CsvReader::~CsvReader()
{
    delete d;
}

void CsvReader::setSource(const QString& source)
{
    d->pos = 0;
    d->src = source;
}

void CsvReader::setTerminator(const QChar& terminator)
{
    d->term = terminator;
}

void CsvReader::setAlternativeTerminator(const QString& terminator)
{
    d->altTerm = terminator;
}

void CsvReader::setEncloseChar(const QChar& enclose)
{
    d->encl = enclose;
}

void CsvReader::setDelimiter(const QChar& delimiter)
{
    d->delim = delimiter;
}

void CsvReader::reset()
{
    d->pos = 0;
}

QStringList CsvReader::parseLine(bool trimSpace)
{
    QStringList resultRow;
    if ((d->pos >= d->src.size() - 1) || d->delim.isNull()
        || ((d->term.isNull()) && (d->altTerm.isEmpty()))
        || d->src.isEmpty())
        return resultRow;

    bool isEnclosed = false;

    QString value;

    QChar first;
    QChar second;
    QString firstSecond;

    int i = d->pos;
    // ¦æ¦¬¦¦¦¬ ¦¬TÀ¦-TÅ¦-¦+¦- ¦¬¦- ¦+¦-¦-¦-TË¦-
    while (i < d->src.size()) {
        first = d->src.at(i);
        if (i < d->src.size() - 1)
            second = d->src.at(i + 1);
        else
            second = QChar();

        firstSecond.append(first);
        firstSecond.append(second);

        //
        if (isEnclosed) {
            if (first == d->encl) {
                if (second == d->encl) {
                    value.append(first);
                    d->pos = ++i;
                } else {
                    isEnclosed = false;
                }
            } else {
                value.append(first);
            }
        } else {
            if (first == d->encl) {
                if (second == d->encl) {
                    value.append(first);
                    d->pos = ++i;
                } else {
                    isEnclosed = true;
                }
            } else if (first == d->delim) {
                if (trimSpace)
                    value = value.trimmed();
                if (!value.compare(d->encl))
                    value.clear();
                resultRow.append(value);
                value.clear();
            } else if (first == d->term) {
                if (trimSpace)
                    value = value.trimmed();
                if (!value.compare(d->encl))
                    value.clear();
                resultRow.append(value);
                value.clear();
                d->pos = (++i);
                break;
            } else if (!firstSecond.compare(d->altTerm)) {
                if (trimSpace)
                    value = value.trimmed();
                if (!value.compare(d->encl))
                    value.clear();
                resultRow.append(value);
                value.clear();
                d->pos = (i+=2);
                break;
            } else {
                value.append(first);
            }
        }

        d->pos = ++i;
        firstSecond.clear();
    }
    if (!value.isEmpty() || isEnclosed || first == d->delim) {
        if (trimSpace)
            value = value.trimmed();
        if (!value.compare(d->encl))
            value.clear();
        resultRow.append(value);
    }
    return resultRow;
}
