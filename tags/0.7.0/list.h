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
#include <QPixmap>
#include <QWidget>
#include <QPoint>

namespace Ui
{
    class List;
}

class QListWidgetItem;

class List : public QWidget
{
    Q_OBJECT
    
public:
    explicit List(int group, QWidget *parent = 0);
    ~List();

    void addTicker(const QString &);

    bool haveTickers() const;

    QString currentTicker() const;

    void setSaveTickers(bool);

    void setIgnoreInput(bool);
    bool ignoreInput() const;

    bool contains(const QPoint &);

    void initialSelect();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    void numberOfItemsChanged();
    QStringList toStringList();
    void save();
    void load();
    void paste();
    void showSaved(bool isSaved);
    void showTickerInfo();
    QPixmap createDragCursor();
    void addItem(const QString &text, bool fix = false);

    enum LoadItem { LoadItemCurrent,
                    LoadItemNext, LoadItemPrevious,
                    LoadItemFirst, LoadItemLast,
                    LoadItemPageUp, LoadItemPageDown };

signals:
    void copyLeft(const QString &ticker);
    void copyRight(const QString &ticker);
    void copyTo(const QString &t, int index);
    void loadTicker(const QString &);
    void dropped(const QString &, const QPoint &);

public slots:
    void clear();

private slots:
    void slotAddOne();
    void slotAddFromFile();
    void slotAddFromClipboard();
    void slotSave();
    void slotExportToFile();
    void slotExportToClipboard();
    void slotSortList();
    void slotLoadPredefined();
    void loadItem(LoadItem = LoadItemCurrent);

private:
    Ui::List *ui;
    int m_section;
    bool m_saveTickers;
    bool m_ignoreInput;
    bool m_dragging;
    QPoint m_startPos;
    QString m_startDragText;
};

inline
void List::setIgnoreInput(bool ignore)
{
    m_ignoreInput = ignore;
}

inline
bool List::ignoreInput() const
{
    return m_ignoreInput;
}

#endif // LIST_H