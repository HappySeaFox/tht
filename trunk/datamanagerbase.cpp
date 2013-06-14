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

#include <QTreeWidgetItem>
#include <QKeySequence>
#include <QStringList>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFrame>

#include "datamanagerbase.h"
#include "ui_datamanagerbase.h"

DataManagerBase::DataManagerBase(QWidget *parent) :
    QDialog(parent)
{
    d = new DataManagerBasePrivate;

    d->ui = new Ui::DataManagerBase;

    d->ui->setupUi(this);

    d->ui->pushClear->setShortcut(QKeySequence::New);
    d->ui->pushDelete->setShortcut(QKeySequence::Delete);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    d->ui->tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    d->ui->tree->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    // layout for extra buttons
    QVBoxLayout *l = new QVBoxLayout(d->ui->widgetButtons);
    l->setContentsMargins(0, 0, 0, 4);
    d->ui->widgetButtons->setLayout(l);
}

DataManagerBase::~DataManagerBase()
{
    delete d->ui;
    delete d;
}

bool DataManagerBase::changed() const
{
    return d->changed;
}

void DataManagerBase::addItem(const QStringList &strings, const QVariant &data, bool edit)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(strings);
    i->setData(0, Qt::UserRole, data);
    i->setFlags(i->flags() | Qt::ItemIsEditable);

    d->ui->tree->addTopLevelItem(i);

    if(edit)
    {
        d->ui->tree->setCurrentItem(i);
        d->ui->tree->editItem(i);
    }
}

void DataManagerBase::addButton(QPushButton *button)
{
    if(!button)
        return;

    // add separator
    if(!d->ui->widgetButtons->layout()->count())
    {
        QFrame *line = new QFrame(d->ui->widgetButtons);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        d->ui->widgetButtons->layout()->addWidget(line);
    }

    // add button
    button->setParent(d->ui->widgetButtons);
    d->ui->widgetButtons->layout()->addWidget(button);

    resetTabOrders();
}

void DataManagerBase::moveItem(int index, int diff)
{
    QTreeWidgetItem *i = d->ui->tree->takeTopLevelItem(index);
    d->ui->tree->insertTopLevelItem(index+diff, i);
    d->ui->tree->setCurrentItem(i, QItemSelectionModel::ClearAndSelect);

    d->changed = true;
}

void DataManagerBase::resetTabOrders()
{
    QWidget *lastWidget = d->ui->pushClear;
    QList<QPushButton *> buttons = d->ui->widgetButtons->findChildren<QPushButton *>();

    foreach(QPushButton *b, buttons)
    {
        QWidget::setTabOrder(lastWidget, b);
        lastWidget = b;
    }

    QWidget::setTabOrder(lastWidget, d->ui->buttonBox);
}

void DataManagerBase::slotDelete()
{
    QTreeWidgetItem *ci = d->ui->tree->currentItem();

    if(!ci)
        return;

    QTreeWidgetItem *i = d->ui->tree->itemBelow(ci);

    if(!i)
        i = d->ui->tree->itemAbove(ci);

    delete ci;

    d->ui->tree->setCurrentItem(i, QItemSelectionModel::ClearAndSelect);

    d->changed = true;
}

void DataManagerBase::slotUp()
{
    QTreeWidgetItem *i = d->ui->tree->currentItem();
    int index = d->ui->tree->indexOfTopLevelItem(i);

    if(!i || index <= 0)
        return;

    moveItem(index, -1);
}

void DataManagerBase::slotDown()
{
    QTreeWidgetItem *i = d->ui->tree->currentItem();
    int index = d->ui->tree->indexOfTopLevelItem(i);

    if(!i || index < 0 || index >= d->ui->tree->topLevelItemCount()-1)
        return;

    moveItem(index, +1);
}

void DataManagerBase::slotClear()
{
    d->ui->tree->clear();
    d->changed = true;
}

void DataManagerBase::slotItemChanged()
{
    d->changed = true;
}
