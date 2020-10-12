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

/*
 *  This CSV reader is grabbed from http://www.prog.org.ru/topic_12337_0.html
 *  It is written by Filonenko Mikhail, and contains no copyright or license
 *  notice
 */

/*
 *  12.06.2013 update: Filonenko Mikhail allowed to license CsvReader
 *  under the terms of LGPL
 */

/*dummy CSV reader for QT4*/
/*version 0.1*/
/*11.1.2009*/

#ifndef CSVREADER_H
#define CSVREADER_H

#include <QStringList>
#include <QChar>

class QString;
class QChar;

class CsvReaderPrivate;

/*
 *  Class to read CSV
 */
class CsvReader
{
public:
    CsvReader();
    CsvReader(const QString& source);
    ~CsvReader();

    void setSource(const QString& source);

    void setTerminator(const QChar& terminator);
    void setAlternativeTerminator(const QString& terminator);
    void setEncloseChar(const QChar& enclose);
    void setDelimiter(const QChar& delimiter);

    void reset();

    QStringList parseLine(bool trimSpace = true);

private:
    CsvReaderPrivate *d;
};

#endif // CSVREADER_H
