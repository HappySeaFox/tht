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

#include <QApplication>
#include <QVBoxLayout>
#include <QEvent>
#include <QFrame>

#include "listdetails.h"
#include "thttools.h"

ListDetails::ListDetails(QWidget *parent) :
    QLabel(parent)
{
    setObjectName("listDetails");

    setStyleSheet(THT_LIST_DETAILS_DEFAULT_STYLESHEET);

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setSpacing(2);
    l->setContentsMargins(2, 2, 3, 3);
    setLayout(l);

    setAttribute(Qt::WA_TransparentForMouseEvents);
    setMinimumWidth(18);
    setFrameShape(QFrame::NoFrame);

    // current ticker
    m_current = new QLabel(this);
    m_current->setObjectName("currentTicker");
    m_current->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_current->setTextFormat(Qt::PlainText);
    m_current->setAttribute(Qt::WA_TranslucentBackground);
    l->addWidget(m_current);

    // separator
    QLabel *line = new QLabel(this);
    line->setObjectName("lineTickersSeparator");
    line->setFixedHeight(1);
    l->addWidget(line);

    // total tickers
    m_total = new QLabel(this);
    m_total->setObjectName("totalTickers");
    m_total->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_total->setTextFormat(Qt::PlainText);
    m_total->setAttribute(Qt::WA_TranslucentBackground);
    l->addWidget(m_total);

    resetStyle();

    qApp->installEventFilter(this);
}

bool ListDetails::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == qApp)
    {
        if(e->type() == THT_STYLE_CHANGE_EVENT_TYPE)
            resetStyle();
    }

    return QObject::eventFilter(obj, e);
}

void ListDetails::resetStyle()
{
    setStyleSheet(THTTools::isStyleApplied() ? QString() : THT_LIST_DETAILS_DEFAULT_STYLESHEET);
}
