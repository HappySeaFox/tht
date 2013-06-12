/*
 *  This CSV reader is grabbed from http://www.prog.org.ru/topic_12337_0.html
 *  It is written by Filonenko Mikhail, and contains no copyright or license
 *  notice
 */

/*
 *  12.06.2013 update: Filonenko Mikhail allowed to license CsvParser
 *  under the terms of LGPL v3+
 */

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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "csvreader.h"
#include <QtCore/QTextDecoder>


CsvParser::CsvParser()
    :src(QString::null)
    , pos(0)
    , delim(QChar(','))
    , encl(QChar('"'))
    , term('\n')
    , altTerm("\r\n")
{

}

CsvParser::CsvParser(const QString& source)
    :src(source)
    , pos(0)
    , delim(QChar(','))
    , encl(QChar('"'))
    , term('\n')
    , altTerm("\r\n")
{
}

CsvParser::~CsvParser()
{
}

void CsvParser::setSource(const QString& source)
{
    pos = 0;
    src = source;
}

void CsvParser::setTerminator(const QChar& terminator)
{
    term = terminator;
}

void CsvParser::setAlternativeTerminator(const QString& terminator)
{
    altTerm = terminator;
}

void CsvParser::setEncloseChar(const QChar& enclose)
{
    encl = enclose;
}

void CsvParser::setDelimiter(const QChar& delimiter)
{
    delim = delimiter;
}

void CsvParser::reset()
{
    pos = 0;
}

QStringList CsvParser::parseLineOld()
{
    QStringList resultRow;
    if ((pos >= src.size() - 1) || delim.isNull()
        || ((term.isNull()) && (altTerm.isEmpty()))
        || src.isEmpty())
        return resultRow;

    QString value;

    GenericWaitingFlag genericWaitingFlag = WaitingForValue;

    QChar first;
    QChar second;
    QString firstSecond;

    int i = pos;
    while (i < src.size()) {
        first = src.at(i);
        if (i < src.size() - 1)
            second = src.at(i + 1);
        else
            second = QChar();
        firstSecond.append(first);
        firstSecond.append(second);

        switch (genericWaitingFlag) {
            // ¦Ö¦+¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦
        case WaitingForValue:
            // ¦ÕTÁ¦¬¦¬ ¦-¦¦TÀ¦-¦-¦¬TÇ¦¬¦-¦-TÎTÉ¦¬¦¦ ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬¦-¦-¦¦
            if (first == encl) {
                // ¦â¦¦¦¬¦¦TÀTÌ ¦¦¦+¦¦¦- ¦¬¦-¦¦TÀTË¦-¦-TÎTÉ¦¬¦¦ ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬¦-¦-¦¦
                genericWaitingFlag = WaitingForEnclose;
                // ¦ÕTÁ¦¬¦¬ TÀ¦-¦¬¦+¦¦¦¬¦¬TÂ¦¦¦¬TÌ
            } else if (first == delim) {
                // ¦ß¦¬TÈ¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬ ¦¦¦+¦¦¦- ¦-¦-¦-¦-¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬TÏ
                resultRow.append(value);
                value.clear();
                // ¦ÕTÁ¦¬¦¬ TÀ¦-¦¬¦+¦¦¦¬¦¬TÂ¦¦¦¬TÌ TÁTÂTÀ¦-¦¦
            } else if (first == term) {
                // ¦ß¦¬TÈ¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬ ¦-TËTÅ¦-¦+¦¬¦- ¦¬¦¬ TÆ¦¬¦¦¦¬¦-
                resultRow.append(value);
                value.clear();
                pos = (++i);
                goto exit_loop;
                // ¦ÕTÁ¦¬¦¬ TÀ¦-¦¬¦+¦¦¦¬¦¬TÂ¦¦¦¬TÌ TÁTÂTÀ¦-¦¦
            } else if (!firstSecond.compare(altTerm)) {
                // ¦ß¦¬TÈ¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬ ¦-TËTÅ¦-¦+¦¬¦- ¦¬¦¬ TÆ¦¬¦¦¦¬¦-
                resultRow.append(value);
                value.clear();
                pos = (i+=2);
                goto exit_loop;
                // ¦Ø¦-¦-TÇ¦¦ ¦+¦-¦¬¦-¦¬¦-TÏ¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦
            } else if (first!=QChar(' ')) {
                // ¦ß¦¬TÈ¦¦¦- ¦¬ ¦-¦¦¦¬¦+¦-¦¦¦- ¦¬¦-¦-¦¦¦¬ TÀ¦-¦¬¦+¦¦¦¬¦¦¦-¦¬TÏ ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬¦¬¦¬ TÁTÂTÀ¦-¦¦ TÁ ¦¬¦-¦¬¦¬TÁTÌTÎ ¦-TÁ¦¦¦¦¦- ¦+TÀTÃ¦¦¦-¦¦¦- ¦-TÁTÂTÀ¦¦TÂ¦¬¦-TÈ¦¦¦¦¦-TÁTÏ
                value.append(first);
                genericWaitingFlag = WaitingForDelimiterOrTerminator;
            }
            break;
            // ¦Ö¦+¦¦¦- ¦¬¦-¦-¦¦ TÀ¦-¦¬¦+¦¦¦¬¦¦¦-¦¬TÏ ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬¦¬¦¬ TÁTÂTÀ¦-¦¦
        case WaitingForDelimiterOrTerminator:
            // ¦ÕTÁ¦¬¦¬ ¦¬¦-¦-¦¦ TÀ¦-¦¬¦+¦¦¦¬¦¦¦-¦¬TÏ TÁTÂTÀ¦-¦¦
            if (first == delim) {
                // ¦×¦-¦¬¦¬TÁTË¦-¦-¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬ ¦-¦-TÇ¦¬¦-¦-¦¦¦- ¦-¦¦¦¬¦+¦-TÂTÌ ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦
                resultRow.append(value);
                value.clear();
                genericWaitingFlag = WaitingForValue;
                // ¦ÕTÁ¦¬¦¬ ¦¬¦-¦-¦¦ TÀ¦-¦¬¦+¦¦¦¬¦¦¦-¦¬TÏ TÁTÂTÀ¦-¦¦ ¦-TËTÅ¦-¦+¦¬¦- ¦¬¦¬ TÆ¦¬¦¦¦¬¦-
            } else if (first == term) {
                // ¦ß¦¬TÈ¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬ ¦-TËTÅ¦-¦+¦¬¦- ¦¬¦¬ TÆ¦¬¦¦¦¬¦-
                resultRow.append(value);
                value.clear();
                pos = (++i);
                goto exit_loop;
                // ¦ÕTÁ¦¬¦¬ ¦¬¦-¦-¦¦ TÀ¦-¦¬¦+¦¦¦¬¦¦¦-¦¬TÏ TÁTÂTÀ¦-¦¦ ¦-TËTÅ¦-¦+¦¬¦- ¦¬¦¬ TÆ¦¬¦¦¦¬¦-
            } else if (!firstSecond.compare(altTerm)) {
                // ¦ß¦¬TÈ¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬ ¦-TËTÅ¦-¦+¦¬¦- ¦¬¦¬ TÆ¦¬¦¦¦¬¦-
                resultRow.append(value);
                value.clear();
                pos = (i+=2);
                goto exit_loop;
                // ¦ÒTÁ¦¦ ¦-TÁTÂ¦-¦¬TÌ¦-¦-¦¦ ¦¬¦¬TÈ¦¦¦- ¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦.
            } else {
                value.append(first);
            }
            break;
            // ¦Ö¦+¦¦¦- ¦¬¦-¦-¦¦ ¦-¦¦TÀ¦-¦-¦¬TÇ¦¬¦-¦-TÎTÉ¦¬¦¦ ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦
        case WaitingForEnclose:
            // ¦ÕTÁ¦¬¦¬ ¦¬¦-¦-¦¦ ¦-¦¦TÀ¦-¦-¦¬TÇ¦¬¦-¦-TÎTÉ¦¬¦¦ ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦
            if (first == encl) {
                // ¦ÕTÁ¦¬¦¬ TÁ¦¬¦¦¦+TÃTÎTÉ¦¬¦¦ ¦¬¦-¦-¦¦ TÂ¦-¦¦¦¦ ¦-¦¦TÀ¦-¦-¦¬TÇ¦¬¦-¦-¦¦TÂ ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦, ¦¬¦-¦-TÇ¦¬TÂ TÍTÂ¦- TÍ¦¦TÀ¦-¦-¦¬TÀ¦-¦-¦-¦-¦¬¦¦
                if (second == encl) {
                    // ¦Ô¦-¦¬¦-¦¬¦-TÏ¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦, ¦¬¦¦TÀ¦¦TÁ¦¦¦-¦¦¦¬¦-¦-¦¦¦- TÍ¦¦TÀ¦-¦-
                    value.append(first);
                    ++i;
                    // ¦á¦¬¦¦¦+TÃTÎTÉ¦¬¦¦ ¦¬¦-¦-¦¦ ¦-¦- TÍ¦¦TÀ¦-¦-¦¬TÀ¦-¦-¦-¦-¦¬¦¦
                } else {
                    // ¦ß¦¬TÈ¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬ ¦-¦¦¦¬¦+¦-¦¦¦- TÀ¦-¦¬¦+¦¦¦¬¦¬TÂ¦¦¦¬TÌ ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬¦¬¦¬ TÁTÂTÀ¦-¦¦ ¦-¦¦¦¬ ¦¬¦-¦¬¦¬TÁ¦¬ ¦-TÁ¦¦¦¦¦- ¦+TÀTÃ¦¦¦-¦¦¦-
                    resultRow.append(value);
                    value.clear();
                    genericWaitingFlag = WaitingForDelimiterOrTerminatorWithoutWriting;
                }
                // ¦ÕTÁ¦¬¦¬ ¦-¦¦TÀ¦-¦-¦¬TÇ¦¬TÂ¦¦¦¬TÌ ¦¦TÉ¦¦ ¦-¦¦ ¦-TÁTÂTÀ¦¦TÂ¦¬¦¬TÁTÏ ¦¬TÀ¦-TÁTÂ¦- ¦+¦-¦¬¦-¦¬¦-TÏ¦¦¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦ ¦¬ ¦¦¦+¦¦¦- ¦¬¦-¦-¦¦¦- ¦-¦¦TÀ¦-¦-¦¬TÇ¦¬TÂ¦¦¦¬TÏ ¦¬¦-¦-TÇ¦¦¦-¦¬TÏ
            } else {
                value.append(first);
            }
            break;
        case WaitingForDelimiterOrTerminatorWithoutWriting:
            // ¦ÕTÁ¦¬¦¬ ¦¬¦-¦-¦¦ TÀ¦-¦¬¦+¦¦¦¬¦¦¦-¦¬TÏ TÁTÂTÀ¦-¦¦
            if (first == delim) {
                // ¦Ý¦-TÇ¦¬¦-¦-¦¦¦- ¦-¦¦¦¬¦+¦-TÂTÌ ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦
                genericWaitingFlag = WaitingForValue;
            } else if (first == term) {
                pos = (++i);
                goto exit_loop;
                // ¦ÕTÁ¦¬¦¬ TÀ¦-¦¬¦+¦¦¦¬¦¬TÂ¦¦¦¬TÌ TÁTÂTÀ¦-¦¦
                // ¦ÕTÁ¦¬¦¬ ¦¬¦-¦-¦¦ TÀ¦-¦¬¦+¦¦¦¬¦¦¦-¦¬TÏ TÁTÂTÀ¦-¦¦ ¦-TËTÅ¦-¦+¦¬¦- ¦¬¦¬ TÆ¦¬¦¦¦¬¦-
            } else if (!firstSecond.compare(altTerm)) {
                pos = (i+=2);
                goto exit_loop;
                // ¦ÒTÁ¦¦ ¦-TÁTÂ¦-¦¬TÌ¦-¦-¦¦ ¦¬¦¬TÈ¦¦¦- ¦- ¦¬¦-¦-TÇ¦¦¦-¦¬¦¦.
            }
            break;
        }

        ++i;
        ++pos;
        firstSecond.clear();
    }
    switch(genericWaitingFlag) {
    case WaitingForValue:
        resultRow.append(value);
        break;
    case WaitingForEnclose:
        resultRow.append(value);
        break;
    case WaitingForDelimiterOrTerminator:
        resultRow.append(value);
        break;
    case WaitingForDelimiterOrTerminatorWithoutWriting:
        break;
    }
    exit_loop:
    return resultRow;
}

QStringList CsvParser::parseLine(bool trimSpace)
{
    QStringList resultRow;
    if ((pos >= src.size() - 1) || delim.isNull()
        || ((term.isNull()) && (altTerm.isEmpty()))
        || src.isEmpty())
        return resultRow;

    bool isEnclosed = false;

    QString value;

    QChar first;
    QChar second;
    QString firstSecond;

    int i = pos;
    // ¦æ¦¬¦¦¦¬ ¦¬TÀ¦-TÅ¦-¦+¦- ¦¬¦- ¦+¦-¦-¦-TË¦-
    while (i < src.size()) {
        first = src.at(i);
        if (i < src.size() - 1)
            second = src.at(i + 1);
        else
            second = QChar();

        firstSecond.append(first);
        firstSecond.append(second);

        //
        if (isEnclosed) {
            if (first == encl) {
                if (second == encl) {
                    value.append(first);
                    pos = ++i;
                } else {
                    isEnclosed = false;
                }
            } else {
                value.append(first);
            }
        } else {
            if (first == encl) {
                if (second == encl) {
                    value.append(first);
                    pos = ++i;
                } else {
                    isEnclosed = true;
                }
            } else if (first == delim) {
                if (trimSpace)
                    value = value.trimmed();
                if (!value.compare(encl))
                    value.clear();
                resultRow.append(value);
                value.clear();
            } else if (first == term) {
                if (trimSpace)
                    value = value.trimmed();
                if (!value.compare(encl))
                    value.clear();
                resultRow.append(value);
                value.clear();
                pos = (++i);
                break;
            } else if (!firstSecond.compare(altTerm)) {
                if (trimSpace)
                    value = value.trimmed();
                if (!value.compare(encl))
                    value.clear();
                resultRow.append(value);
                value.clear();
                pos = (i+=2);
                break;
            } else {
                value.append(first);
            }
        }

        pos = ++i;
        firstSecond.clear();
    }
    if (!value.isEmpty() || isEnclosed || first == delim) {
        if (trimSpace)
            value = value.trimmed();
        if (!value.compare(encl))
            value.clear();
        resultRow.append(value);
    }
    return resultRow;
}
