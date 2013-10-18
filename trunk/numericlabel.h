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

#ifndef NUMERICLABEL_H
#define NUMERICLABEL_H

#include <QLabel>
#include <QHash>

class QPixmap;

class NumericLabel : public QLabel
{
public:
    NumericLabel(QWidget *parent = 0);
    NumericLabel(uint value = 0, QWidget *parent = 0);
    ~NumericLabel();

    void setValue(uint val);
    uint value() const;

private:
    void init(uint val);

private:
    uint m_value;
    QHash<uint, QPixmap> m_cache;
};

inline
uint NumericLabel::value() const
{
    return m_value;
}

#endif // NUMERICLABEL_H
