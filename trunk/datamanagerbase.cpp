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
    QDialog(parent),
    ui(new Ui::DataManagerBase),
    m_changed(false)
{
    ui->setupUi(this);

    ui->pushClear->setShortcut(QKeySequence::New);
    ui->pushDelete->setShortcut(QKeySequence::Delete);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->tree->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    // layout for extra buttons
    QVBoxLayout *l = new QVBoxLayout(ui->widgetButtons);
    l->setContentsMargins(0, 0, 0, 4);
    ui->widgetButtons->setLayout(l);
}

DataManagerBase::~DataManagerBase()
{
    delete ui;
}

void DataManagerBase::addItem(const QStringList &strings, const QVariant &data, bool edit)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(strings);
    i->setData(0, Qt::UserRole, data);
    i->setFlags(i->flags() | Qt::ItemIsEditable);

    ui->tree->addTopLevelItem(i);

    if(edit)
    {
        ui->tree->setCurrentItem(i);
        ui->tree->editItem(i);
    }
}

void DataManagerBase::addButton(QPushButton *button)
{
    if(!button)
        return;

    // add separator
    if(!ui->widgetButtons->layout()->count())
    {
        QFrame *line = new QFrame(ui->widgetButtons);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        ui->widgetButtons->layout()->addWidget(line);
    }

    // add button
    button->setParent(ui->widgetButtons);
    ui->widgetButtons->layout()->addWidget(button);

    resetTabOrders();
}

void DataManagerBase::moveItem(int index, int diff)
{
    QTreeWidgetItem *i = ui->tree->takeTopLevelItem(index);
    ui->tree->insertTopLevelItem(index+diff, i);
    ui->tree->setCurrentItem(i, QItemSelectionModel::ClearAndSelect);

    m_changed = true;
}

void DataManagerBase::resetTabOrders()
{
    QWidget *lastWidget = ui->pushClear;
    QList<QPushButton *> buttons = ui->widgetButtons->findChildren<QPushButton *>();

    foreach(QPushButton *b, buttons)
    {
        QWidget::setTabOrder(lastWidget, b);
        lastWidget = b;
    }

    QWidget::setTabOrder(lastWidget, ui->buttonBox);
}

void DataManagerBase::slotDelete()
{
    QTreeWidgetItem *ci = ui->tree->currentItem();

    if(!ci)
        return;

    QTreeWidgetItem *i = ui->tree->itemBelow(ci);

    if(!i)
        i = ui->tree->itemAbove(ci);

    delete ci;

    ui->tree->setCurrentItem(i, QItemSelectionModel::ClearAndSelect);

    m_changed = true;
}

void DataManagerBase::slotUp()
{
    QTreeWidgetItem *i = ui->tree->currentItem();
    int index = ui->tree->indexOfTopLevelItem(i);

    if(!i || index <= 0)
        return;

    moveItem(index, -1);
}

void DataManagerBase::slotDown()
{
    QTreeWidgetItem *i = ui->tree->currentItem();
    int index = ui->tree->indexOfTopLevelItem(i);

    if(!i || index < 0 || index >= ui->tree->topLevelItemCount()-1)
        return;

    moveItem(index, +1);
}

void DataManagerBase::slotClear()
{
    ui->tree->clear();
    m_changed = true;
}
