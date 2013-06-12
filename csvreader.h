/*
 *  This CSV reader is grabbed from http://www.prog.org.ru/topic_12337_0.html
 *  It is written by Filonenko Mikhail, and contains no copyright or license
 *  notice
 */

/*dummy CSV reader for QT4*/
/*version 0.1*/
/*11.1.2009*/
#ifndef CSV_H
#define CSV_H

//#include "myclass_api.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>

class /*MYCLASS_API*/ CsvParser /*: public QObject*/
{
    /*Q_OBJECT*/
public:
    enum GenericWaitingFlag {
        WaitingForValue,
        WaitingForEnclose,
        WaitingForDelimiterOrTerminator,
        WaitingForDelimiterOrTerminatorWithoutWriting
    };

    CsvParser();
    CsvParser(const QString& source);
    ~CsvParser();

    void setSource(const QString& source);

    void setTerminator(const QChar& terminator);
    void setAlternativeTerminator(const QString& terminator);
    void setEncloseChar(const QChar& enclose);
    void setDelimiter(const QChar& delimiter);

    void reset();
    QStringList parseLineOld();

    QStringList parseLine(bool trimSpace = true);
private:
    QString src;
    int pos;
    QChar delim;
    QChar encl;
    QChar term;
    QString altTerm;
};

#endif // CSV_H
