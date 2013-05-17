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
    NoEditorDelegate(QObject *parent) :
        QStyledItemDelegate(parent)
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
    //: Noun. Means "Points of linkage"
    setWindowTitle(tr("Link points"));

    //: Means "Add current link points" (in the plural)
    ui->pushAdd->setText(tr("Add current"));

    ui->tree->headerItem()->setText(1, tr("Link points"));
    ui->tree->setWhatsThis(QString("<a href=\"http://www.youtube.com/watch?v=1PlpDwhgLEs\">%1</a>").arg(tr("Open YouTube tutorial")));

    ui->tree->setItemDelegateForColumn(1, new NoEditorDelegate(ui->tree));

    if(m_currentLinks.isEmpty())
        ui->pushAdd->setEnabled(false);

    QList<LinkPoint> linkpoints = SETTINGS_GET_LINKS(SETTING_LINKS);

    foreach(LinkPoint lp, linkpoints)
    {
        addLinkPoint(lp);
    }

    ui->tree->setCurrentItem(ui->tree->topLevelItem(0));
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

    points.reserve(10 * lp.points.size()); // assume 10 bytes per point: "(xxx,yyy),"

    foreach(QPoint p, lp.points)
    {
        points += QString("(%1,%2),").arg(p.x()).arg(p.y());
    }

    points.chop(1);
    points.squeeze();

    addItem(QStringList() << lp.name << points, QVariant::fromValue(lp.points), edit);
}

void LinkPointManager::slotAdd()
{
    addLinkPoint(LinkPoint(tr("New points"), m_currentLinks), true);
    m_changed = true;
}
