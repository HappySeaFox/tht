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

#include <QStringList>
#include <QWidget>

namespace Ui {
class Widget;
}

class NetworkAccess;

struct Ticker
{
    Ticker()
    {
        cap = 0;
    }

    QString ticker;
    QString company;
    QString sector;
    QString industry;
    QString exchange;
    QString country;

    double cap;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    virtual void closeEvent(QCloseEvent *);

private:
    bool writeData(const Ticker &);
    void message(const QString &, bool = true);

private slots:
    bool commit();
    void slotFinished();
    void slotFinishedExchange();
    void slotGet();

private:
    Ui::Widget *ui;
    NetworkAccess *m_net;
    QStringList m_oldTickers;
    QStringList m_tickersForExchange;
    QString m_ts;
    bool m_running;
    bool m_auto;
};

#endif // WIDGET_H
