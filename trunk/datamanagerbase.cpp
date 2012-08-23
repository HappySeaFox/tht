#include <QWhatsThisClickedEvent>
#include <QDesktopServices>
#include <QTreeWidgetItem>
#include <QKeySequence>
#include <QStringList>
#include <QUrl>

#include "datamanagerbase.h"
#include "ui_datamanagerbase.h"

DataManagerBase::DataManagerBase(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataManagerBase),
    m_changed(false)
{
    ui->setupUi(this);

    ui->pushClear->setShortcut(QKeySequence::New);
    ui->tree->header()->setResizeMode(QHeaderView::ResizeToContents);
}

DataManagerBase::~DataManagerBase()
{
    delete ui;
}

bool DataManagerBase::event(QEvent *e)
{
    if(e->type() == QEvent::WhatsThisClicked)
    {
        QWhatsThisClickedEvent *ce = static_cast<QWhatsThisClickedEvent *>(e);

        if(ce)
            QDesktopServices::openUrl(QUrl(ce->href()));
    }

    return QDialog::event(e);
}

void DataManagerBase::addItem(const QStringList &strings, const QVariant &data, bool edit)
{
    QTreeWidgetItem *i = new QTreeWidgetItem(strings);
    i->setData(0, Qt::UserRole, data);
    i->setFlags(i->flags() | Qt::ItemIsEditable);

    ui->tree->addTopLevelItem(i);

    if(edit)
    {
        ui->tree->setCurrentItem(i, QItemSelectionModel::ClearAndSelect);
        ui->tree->editItem(i);
    }
}

void DataManagerBase::moveItem(int index, int diff)
{
    QTreeWidgetItem *i = ui->tree->takeTopLevelItem(index);
    ui->tree->insertTopLevelItem(index+diff, i);
    ui->tree->setCurrentItem(i, QItemSelectionModel::ClearAndSelect);

    m_changed = true;
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
