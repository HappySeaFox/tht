/*
 * This file is part of THT-lib.
 *
 * THT-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation.
 *
 * THT-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NOEDITORDELEGATE_H
#define NOEDITORDELEGATE_H

#include <QStyledItemDelegate>

/*
 *  Styled delegate which will prevent the editor from opening
 */
class NoEditorDelegate : public QStyledItemDelegate
{
public:
    NoEditorDelegate(QObject *parent);

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // NOEDITORDELEGATE_H
