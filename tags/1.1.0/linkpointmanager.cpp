#include <QStyledItemDelegate>
#include <QHeaderView>

#include "linkpointmanager.h"
#include "settings.h"

#include "ui_linkpointmanager.h"

class NoEditorDelegate : public QStyledItemDelegate
{
public:
    NoEditorDelegate(QObject *parent)
        : QStyledItemDelegate(parent)
    {}

    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        Q_UNUSED(parent)
        Q_UNUSED(option)
        Q_UNUSED(index)

        return 0;
    }
};

LinkPointManager::LinkPointManager(const QList<QPoint> &currentLinks, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinkPointManager),
    m_currentLinks(currentLinks),
    m_changed(false)
{
    ui->setupUi(this);

    ui->pushClear->setShortcut(QKeySequence::New);

    ui->tree->setItemDelegateForColumn(1, new NoEditorDelegate(ui->tree));
    ui->tree->header()->setResizeMode(QHeaderView::ResizeToContents);

    if(m_currentLinks.isEmpty())
        ui->pushAdd->setEnabled(false);

    QList<LinkPoint> linkpoints = Settings::instance()->links();

    foreach(LinkPoint lp, linkpoints)
    {
        addItem(lp);
    }

    ui->tree->setCurrentItem(ui->tree->topLevelItem(0), QItemSelectionModel::ClearAndSelect);
}

LinkPointManager::~LinkPointManager()
{
    delete ui;
}

QList<LinkPoint> LinkPointManager::links() const
{
    QList<LinkPoint> linkpoints;
    QTreeWidgetItem *i = ui->tree->topLevelItem(0);

    while(i)
    {
        linkpoints.append(LinkPoint(i->text(0), i->data(0, Qt::UserRole).value<QList<QPoint> >()));

        i = ui->tree->itemBelow(i);
    }

    return linkpoints;
}

void LinkPointManager::addItem(const LinkPoint &lp, bool edit)
{
    QString points;

    foreach(QPoint p, lp.points)
    {
        points += QString("(%1,%2),").arg(p.x()).arg(p.y());
    }

    points.chop(1);

    QTreeWidgetItem *i = new QTreeWidgetItem(ui->tree, QStringList() << lp.name << points);
    i->setData(0, Qt::UserRole, QVariant::fromValue(lp.points));
    i->setFlags(i->flags() | Qt::ItemIsEditable);

    ui->tree->addTopLevelItem(i);

    if(edit)
    {
        ui->tree->setCurrentItem(i, QItemSelectionModel::ClearAndSelect);
        ui->tree->editItem(i);
    }
}

void LinkPointManager::moveItem(QTreeWidgetItem *i, int index, int diff)
{
    ui->tree->takeTopLevelItem(index);
    ui->tree->insertTopLevelItem(index+diff, i);
    ui->tree->setCurrentItem(i, QItemSelectionModel::ClearAndSelect);

    m_changed = true;
}

void LinkPointManager::slotAdd()
{
    addItem(LinkPoint(tr("New points"), m_currentLinks), true);
    m_changed = true;
}

void LinkPointManager::slotDelete()
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

void LinkPointManager::slotUp()
{
    QTreeWidgetItem *i = ui->tree->currentItem();
    int index = ui->tree->indexOfTopLevelItem(i);

    if(!i || index <= 0)
        return;

    moveItem(i, index, -1);
}

void LinkPointManager::slotDown()
{
    QTreeWidgetItem *i = ui->tree->currentItem();
    int index = ui->tree->indexOfTopLevelItem(i);

    if(!i || index < 0 || index >= ui->tree->topLevelItemCount()-1)
        return;

    moveItem(i, index, +1);
}

void LinkPointManager::slotClear()
{
    ui->tree->clear();
    m_changed = true;
}
