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

#ifndef LISTDETAILS_H
#define LISTDETAILS_H

#include <QLabel>

#define THT_LIST_DETAILS_DEFAULT_STYLESHEET \
"QLabel#listDetails { border: 1px solid gray; background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffefef, stop:0.35 #f7db45, stop:0.65 #f7db45, stop:1 #ffefef); }" \
"QLabel#listDetails QLabel#currentTicker { color: black; }" \
"QLabel#listDetails QLabel#lineTickersSeparator { background-color: gray; }" \
"QLabel#listDetails QLabel#totalTickers { color: black; }"

class ListDetails : public QLabel
{
public:
    ListDetails(QWidget *parent = 0);

    QString totalText() const
    {
        return m_total->text();
    }

    void setTotal(int total)
    {
        m_total->setNum(total);
    }

    void setCurrent(int current)
    {
        m_current->setNum(current);
    }

protected:
    virtual bool eventFilter(QObject *obj, QEvent *e);

private:
    QLabel *m_total, *m_current;
};

#endif // LISTDETAILS_H
