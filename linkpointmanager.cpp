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

LinkPointManager::LinkPointManager(const LinkPointSession &currentLinks, QWidget *parent) :
    DataManagerBase(parent),
    m_currentLinks(currentLinks)
{
    //: Noun. Means "Points of linkage"
    setWindowTitle(tr("Link points"));

    //: Means "Add current link points" (in the plural)
    buttonAdd()->setText(tr("Add current"));

    QTreeWidget *t = tree();

    t->headerItem()->setText(1, tr("Link points"));
    t->setWhatsThis(QString("<a href=\"http://www.youtube.com/watch?v=1PlpDwhgLEs\">%1</a>").arg(tr("Open YouTube tutorial")));

    t->setItemDelegateForColumn(1, new NoEditorDelegate(t));

    if(m_currentLinks.windows.isEmpty())
        buttonAdd()->setEnabled(false);

    QList<LinkPointSession> linkpoints = SETTINGS_GET_LINKS(SETTING_LINKS);

    foreach(LinkPointSession lp, linkpoints)
    {
        addLinkPoint(lp);
    }

    t->setCurrentItem(t->topLevelItem(0));
}

LinkPointManager::~LinkPointManager()
{}

QList<LinkPointSession> LinkPointManager::links() const
{
    QList<LinkPointSession> linkpoints;
    QTreeWidget *t = tree();
    QTreeWidgetItem *i = t->topLevelItem(0);

    while(i)
    {
        linkpoints.append(LinkPointSession(i->text(0), i->data(0, Qt::UserRole).value<QList<LinkedWindow> >()));

        i = t->itemBelow(i);
    }

    return linkpoints;
}

void LinkPointManager::addLinkPoint(const LinkPointSession &lps, bool edit)
{
    QString points;

    points.reserve(10 * lps.windows.size()); // assume 10 bytes per point: "(xxx,yyy),"

    foreach(LinkedWindow w, lps.windows)
    {
        points += QString("%1%2,%3%4,")
                        .arg(w.master ? '[' : '(')
                        .arg(w.point.x())
                        .arg(w.point.y())
                        .arg(w.master ? ']' : ')');
    }

    points.chop(1);
    points.squeeze();

    addItem(QStringList() << lps.name << points, QVariant::fromValue(lps.windows), edit);
}

void LinkPointManager::slotAdd()
{
    addLinkPoint(m_currentLinks, true);
    setChanged(true);
}
