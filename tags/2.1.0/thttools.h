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

#ifndef THTTOOLS_H
#define THTTOOLS_H

#include <QPixmap>
#include <QString>
#include <QEvent>
#include <QSize>

static const int THT_STYLE_CHANGE_EVENT_TYPE = QEvent::User + 2;

class QWidget;

class THTTools
{
public:
    static QString pointsOfConnectionTitle();

    static QString addTickersTitle();

    static QString exportTickersTitle();

    static QString aboutThtTitle();

    enum ResetStyleOnErrorType { ResetStyleOnError, DontResetStyleOnError };

    static void resetStyle(ResetStyleOnErrorType rt);

    static bool isStyleApplied();

    static QPixmap renderButtonWithPencil(QWidget *button, const QSize &size = QSize(16, 16));

private:
    THTTools();

private:
    static bool m_isStyleApplied;
};

inline
QString THTTools::pointsOfConnectionTitle()
{
    //: Noun in the plural
    return QObject::tr("Points of connection");
}

inline
QString THTTools::addTickersTitle()
{
    //: This is the label on a button that user pushes to issue the command. Ticker is a short company name, see http://www.investopedia.com/terms/t/tickersymbol.asp . The common practice is to borrow "ticker" from English and incorporate into your language (see http://en.wikipedia.org/wiki/Loanword)
    return QObject::tr("Add tickers");
}

inline
QString THTTools::exportTickersTitle()
{
    //: This is the label on a button that user pushes to issue the command. Ticker is a short company name, see http://www.investopedia.com/terms/t/tickersymbol.asp . The common practice is to borrow "ticker" from English and incorporate into your language (see http://en.wikipedia.org/wiki/Loanword)
    return QObject::tr("Export tickers");
}

inline
QString THTTools::aboutThtTitle()
{
    //: "THT" is the name of the application
    return QObject::tr("About THT");
}

#endif // THTTOOLS_H
