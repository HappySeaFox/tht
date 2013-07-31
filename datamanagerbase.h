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

#ifndef DATAMANAGERBASE_H
#define DATAMANAGERBASE_H

#include <QDialog>

class QTreeWidget;
class QPushButton;
class QStringList;
class QVariant;

class DataManagerBasePrivate;

/*
 *  Key-value editor
 *
 *  Steps to use:
 *      1) subclass DataManagerBase
 *      2) call setAddButtonText("Add my value")
 *      3) fill the view with addItem(...)
 *      4) implement slotAdd()
 *
 *  See FinvizUrlManager for the detailed example
 */
class DataManagerBase : public QDialog
{
    Q_OBJECT

public:
    explicit DataManagerBase(QWidget *parent = 0);
    virtual ~DataManagerBase();

    /*
     *  Is anything changed? Use this method to determine
     *  if you need to save the changed data
     */
    bool changed() const;

protected:
    /*
     *  Add new item to the list. If 'edit' is 'true', then the editor is opened
     *  on the newly added item
     */
    void addItem(const QStringList &strings, const QVariant &data, bool edit = false);

    /*
     *  Add your own button to the container
     */
    void addButton(QPushButton *);

    /*
     *  Force the data to be changed or not
     */
    void setChanged(bool);

    /*
     *  Pointer to the tree with items
     */
    QTreeWidget *tree() const;

    /*
     *  Pointer to 'Add' button. Use this pointer to set
     *  a button text which is empty by default
     */
    QPushButton *buttonAdd() const;

private:
    void moveItem(int index, int diff);
    void resetTabOrders();

protected slots:
    /*
     *  'Add' button has been clicked. Add a new item to the view with addItem()
     *   and call setChecked(true)
     */
    virtual void slotAdd() = 0;

private slots:
    void slotDelete();
    void slotUp();
    void slotDown();
    void slotClear();
    void slotItemChanged();

private:
    DataManagerBasePrivate *d;
};

#endif // DATAMANAGERBASE_H
