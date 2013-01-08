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

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

#include "listwidget.h"
#include "settings.h"

ListWidget::ListWidget(QWidget *parent) :
    QListWidget(parent)
{}

void ListWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasFormat("text/plain"))
    {
        qDebug("Accepting dragged MIME in list");
        e->acceptProposedAction();
    }
}

void ListWidget::dragMoveEvent(QDragMoveEvent *e)
{
    e->acceptProposedAction();
}

void ListWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void ListWidget::dropEvent(QDropEvent *e)
{
    e->acceptProposedAction();

    QString ticker = e->mimeData()->text().toUpper();

    if(Settings::instance()->tickerValidator().exactMatch(ticker))
        emit dropped(ticker);
    else
        qDebug("Dropped ticker \"%s\" doesn't match the regexp", qPrintable(ticker));
}
