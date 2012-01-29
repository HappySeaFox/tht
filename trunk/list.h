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

#ifndef LIST_H
#define LIST_H

#include <QStringList>
#include <QRegExp>
#include <QWidget>

namespace Ui
{
    class List;
}

class QListWidgetItem;

class List : public QWidget
{
    Q_OBJECT
    
public:
    explicit List(const QString &section, QWidget *parent = 0);
    ~List();

    void setSectionToSaveTo(const QString &name);
    QString sectionToSaveTo() const;

    QString currentTicker() const;

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    void numberOfItemsChanged();
    QStringList toStringList();
    void save();
    void load();
    void paste();

signals:
    void moveLeft(const QString &ticker);
    void moveRight(const QString &ticker);
    void loadTicker(const QString &);

private slots:
    void slotAdd();
    void slotClear();
    void slotSaveAs();
    void slotCurrentItemChanged(QListWidgetItem *);

private:
    Ui::List *ui;
    QString m_section;
    QRegExp m_rxTicker;
};

inline
QString List::sectionToSaveTo() const
{
    return m_section;
}

#endif // LIST_H
