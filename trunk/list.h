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

#include "ticker.h"

class QAbstractItemDelegate;
class QListWidgetItem;
class QStringList;
class QAction;
class QLabel;
class QMenu;
class QUrl;

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

    enum FixName { Fix, DontFix };

    void addTickers(const QStringList &tk, FixName fix);

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
    virtual void moveEvent(QMoveEvent *);

private:
    void numberOfItemsChanged();
    enum toStringListFlags { WithExtraData = 1 };
    QStringList toStringList(int flags = 0);
    void save();
    void load();
    void showSaved(bool isSaved);
    void showTickerInfo();
    QPixmap createDragCursor();
    enum CheckForDups { CheckDups, DontCheckDups };
    bool addItem(const QString &text, FixName, CheckForDups);
    void changePriority(int);
    void setPriority(int);
    void resizeNumberLabel();
    void moveNumbersLabel();
    void undo();
    void addFromFinviz(const QUrl &);
    void showFinvizSelector();
    void openTickerInBrowser(const QString &baseUrl, const QString &ticker, FixName fix);
    void changeComment();
    void showComment();
    Ticker currentTickerInfo() const;

    enum LoadItem { LoadItemCurrent,
                    LoadItemNext, LoadItemPrevious,
                    LoadItemFirst, LoadItemLast,
                    LoadItemPageUp, LoadItemPageDown };

    enum MoveItem { MoveItemNext, MoveItemPreviuos,
                    MoveItemFirst, MoveItemLast,
                    MoveItemPageUp, MoveItemPageDown };

    void moveItem(MoveItem);

signals:
    void copyLeft(const Ticker &ticker);
    void copyRight(const Ticker &ticker);
    void copyTo(const Ticker &ticker, int index);
    void loadTicker(const QString &);
    void tickerDropped(const Ticker &ticker, const QPoint &);
    void tickerMoving(const QPoint &);
    void tickerCancelled();
    void showNeighbors(const QString &);
    void needRebuildFinvizMenu();

public slots:
    void addTicker(const Ticker &);
    void clear();
    void startSearching();
    void stopSearching();

private slots:
    void slotAddOne();
    void slotAddTicker(const QString &t);
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
    Ticker m_startDragTicker;
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
