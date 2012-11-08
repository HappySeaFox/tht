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
#include <QList>

#include "listitem.h"

class QListWidgetItem;
class QStringList;
class QAction;
class QLabel;
class QMenu;

class ListDetails;

namespace Ui
{
    class List;
}

class List : public QWidget
{
    Q_OBJECT

public:
    explicit List(int group, QWidget *parent = 0);
    ~List();

    bool hasTickers() const;

    QString currentTicker() const;

    ListItem::Priority currentPriority() const;

    void setSaveTickers(bool);

    void showHeader(bool);

    void setIgnoreInput(bool);
    bool ignoreInput() const;

    bool contains(const QPoint &);

    void initialSelect(bool alsoSetFocus = true);

    void removeDuplicates();

    bool searching() const;

    void reconfigureMiniTickerEntry();
    void focusMiniTickerEntry();

    void rebuildFinvizMenu();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
    virtual void moveEvent(QMoveEvent *);

private:
    void numberOfItemsChanged();
    QStringList toStringList(bool withPriority);
    void save();
    void load();
    void showSaved(bool isSaved);
    void showTickerInfo();
    QPixmap createDragCursor();
    enum FixName { Fix, DontFix };
    enum CheckForDups { CheckDups, DontCheckDups };
    bool addItem(const QString &text, FixName, CheckForDups);
    void addTickers(const QStringList &tk, FixName fix);
    void changePriority(int);
    void setPriority(int);
    void resizeNumberLabel();
    void moveNumbersLabel();
    void undo();
    void addFromFinviz(const QUrl &);
    void showFinvizSelector();

    enum LoadItem { LoadItemCurrent,
                    LoadItemNext, LoadItemPrevious,
                    LoadItemFirst, LoadItemLast,
                    LoadItemPageUp, LoadItemPageDown };

    enum MoveItem { MoveItemNext, MoveItemPreviuos,
                    MoveItemFirst, MoveItemLast,
                    MoveItemPageUp, MoveItemPageDown };

    void moveItem(MoveItem);

signals:
    void copyLeft(const QString &ticker, ListItem::Priority);
    void copyRight(const QString &ticker, ListItem::Priority);
    void copyTo(const QString &t, ListItem::Priority, int index);
    void loadTicker(const QString &);
    void tickerDropped(const QString &, ListItem::Priority, const QPoint &);
    void tickerMoving(const QPoint &);
    void tickerCancelled();
    void showNeighbors(const QString &);
    void needRebuildFinvizMenu();

public slots:
    void addTicker(const QString &, ListItem::Priority p = ListItem::PriorityNormal);
    void clear();
    void startSearching();
    void stopSearching();

private slots:
    void slotAddOne();
    void slotAddFromFile();
    void slotSave();
    void slotExportToFile();
    void slotExportToClipboard();
    void slotSortList();
    void slotResetPriorities();
    void slotResetPriority();
    void slotSearchTicker(const QString &);
    void slotSearchTickerNext();
    void slotFocusUp();
    void slotAddFromFinviz();
    void slotManageFinvizUrls();
    void slotCurrentRowChanged(int);
    void loadItem(LoadItem = LoadItemCurrent);
    void paste();
    void headerCancelled();
    void headerAccepted();
    void changeHeader();

private:
    Ui::List *ui;
    int m_section;
    bool m_saveTickers;
    bool m_ignoreInput;
    bool m_dragging;
    QPoint m_startPos;
    QString m_startDragText;
    ListItem::Priority m_startDragPriority;
    ListDetails *m_numbers;
    QAbstractItemDelegate *m_oldDelegate, *m_persistentDelegate;
    QList<QListWidgetItem *> m_foundItems;
    QStringList m_oldTickers;
    QMenu *m_finvizMenu;
    QAction *m_changeTitle;
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
