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

#ifndef FINVIZURLMANAGER_H
#define FINVIZURLMANAGER_H

#include <QList>

#include "datamanagerbase.h"

class QTreeWidgetItem;
class FinvizUrl;

class FinvizUrlManager : public DataManagerBase
{
    Q_OBJECT
    
public:
    explicit FinvizUrlManager(QWidget *parent = 0);
    ~FinvizUrlManager();

    QList<FinvizUrl> urls() const;

private:
    void addFinvizUrl(const FinvizUrl &fu, bool edit = false);

private slots:
    virtual void slotAdd();
    void slotCheckItem(QTreeWidgetItem *, int);
    void slotAccessClicked();
};

#endif // FINVIZURLMANAGER_H
