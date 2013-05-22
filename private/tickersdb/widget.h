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
#include <QList>
#include <QDate>

class NetworkAccess;

namespace Ui {
class Widget;
}

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
    bool writeTicker(const Ticker &);
    bool writeFomcDate(const QDate &);
    void message(const QString &msg, bool activate = true);
    void rereadFomcDates();
    void proceedToTickers();
    bool addFomcDates(const QString &data, const QString &title);

private slots:
    bool commit();
    void slotFinished();
    void slotFinishedExchange();
    void slotFinishedFomc();
    void slotGet();

private:
    Ui::Widget *ui;
    NetworkAccess *m_net;
    QStringList m_oldTickers;
    QStringList m_tickersForExchange;
    QString m_ts;
    bool m_running;
    bool m_auto;
    QList<QDate> m_fomcDates;
};

#endif // WIDGET_H
