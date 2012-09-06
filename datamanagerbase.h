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

#ifndef DATAMANAGERBASE_H
#define DATAMANAGERBASE_H

#include <QDialog>

class QStringList;
class QVariant;

namespace Ui
{
    class DataManagerBase;
}

class DataManagerBase : public QDialog
{
    Q_OBJECT

public:
    explicit DataManagerBase(QWidget *parent = 0);
    virtual ~DataManagerBase();

    bool changed() const;

protected:
    void addItem(const QStringList &strings, const QVariant &data, bool edit = false);

private:
    void moveItem(int index, int diff);

private slots:
    virtual void slotAdd() = 0;
    void slotDelete();
    void slotUp();
    void slotDown();
    void slotClear();
    void slotItemChanged();

protected:
    Ui::DataManagerBase *ui;
    bool m_changed;
};

inline
bool DataManagerBase::changed() const
{
    return m_changed;
}

inline
void DataManagerBase::slotItemChanged()
{
    m_changed = true;
}

#endif // DATAMANAGERBASE_H
