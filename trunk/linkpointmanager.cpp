#include <QStyledItemDelegate>
#include <QStringList>
#include <QHeaderView>
#include <QVariant>

#include "linkpointmanager.h"
#include "settings.h"

#include "ui_datamanagerbase.h"

namespace
{

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

}

LinkPointManager::LinkPointManager(const QList<QPoint> &currentLinks, QWidget *parent) :
    DataManagerBase(parent),
    m_currentLinks(currentLinks)
{
    setWindowTitle(tr("Link points"));

    /*: Means "Add current link points (in the plural)" */
    ui->pushAdd->setText(tr("Add current"));

    ui->tree->headerItem()->setText(1, tr("Link points"));
    ui->tree->setWhatsThis(QString("<a href=\"http://www.youtube.com/watch?v=FnXsTW3RQVI\">%1</a>").arg(tr("Open YouTube tutorial")));

    ui->tree->setItemDelegateForColumn(1, new NoEditorDelegate(ui->tree));

    if(m_currentLinks.isEmpty())
        ui->pushAdd->setEnabled(false);

    QList<LinkPoint> linkpoints = Settings::instance()->links();

    foreach(LinkPoint lp, linkpoints)
    {
        addLinkPoint(lp);
    }

    ui->tree->setCurrentItem(ui->tree->topLevelItem(0), QItemSelectionModel::ClearAndSelect);
}

LinkPointManager::~LinkPointManager()
{}

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

void LinkPointManager::addLinkPoint(const LinkPoint &lp, bool edit)
{
    QString points;

    foreach(QPoint p, lp.points)
    {
        points += QString("(%1,%2),").arg(p.x()).arg(p.y());
    }

    points.chop(1);

    addItem(QStringList() << lp.name << points, QVariant::fromValue(lp.points), edit);
}

void LinkPointManager::slotAdd()
{
    addLinkPoint(LinkPoint(tr("New points"), m_currentLinks), true);
    m_changed = true;
}
