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

#include <QResizeEvent>
#include <QLabel>

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
    virtual void resizeEvent(QResizeEvent *e)
    {
        Q_UNUSED(e)

        resetBackground();
    }

private:
    void resetBackground();

private:
    QLabel *m_total, *m_current;
};

#endif // LISTDETAILS_H
