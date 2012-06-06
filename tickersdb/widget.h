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

#ifndef WIDGET_H
#define WIDGET_H

#include <QSqlDatabase>
#include <QStringList>
#include <QWidget>

namespace Ui {
class Widget;
}

class NetworkAccess;

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    bool writeData(const QString &, const QString &, const QString &, const QString &);

private slots:
    void slotFinished();
    void slotGet();

private:
    Ui::Widget *ui;
    NetworkAccess *m_net;
    QSqlDatabase db;
    QStringList oldTickers;
};

#endif // WIDGET_H
