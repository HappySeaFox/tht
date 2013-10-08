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

#ifndef STOCKSINPLAYURLMANAGER_H
#define STOCKSINPLAYURLMANAGER_H

#include <QList>

#include "datamanagerbase.h"

class QTreeWidgetItem;
class StocksInPlayUrl;

class StocksInPlayUrlManager : public DataManagerBase
{
    Q_OBJECT
    
public:
    explicit StocksInPlayUrlManager(QWidget *parent = 0);
    ~StocksInPlayUrlManager();

    QList<StocksInPlayUrl> urls() const;

private:
    void addStocksInPlayUrl(const StocksInPlayUrl &su, bool edit = false);

protected slots:
    virtual void slotAdd();

private slots:
    void slotCheckItem(QTreeWidgetItem *, int);
    void slotAccessClicked();
};

#endif // STOCKSINPLAYURLMANAGER_H
