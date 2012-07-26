#ifndef LINKPOINTMANAGER_H
#define LINKPOINTMANAGER_H

#include <QDialog>
#include <QPoint>
#include <QList>

#include "settings.h"

namespace Ui {
class LinkPointManager;
}

class QTreeWidgetItem;

class LinkPointManager : public QDialog
{
    Q_OBJECT
    
public:
    explicit LinkPointManager(const QList<QPoint> &currentLinks, QWidget *parent = 0);
    ~LinkPointManager();

    bool changed() const;

    QList<LinkPoint> links() const;

private:
    void addItem(const LinkPoint &, bool edit = false);
    void moveItem(QTreeWidgetItem *, int index, int diff);

private slots:
    void slotAdd();
    void slotDelete();
    void slotUp();
    void slotDown();
    void slotClear();
    void slotItemChanged();

private:
    Ui::LinkPointManager *ui;
    QList<QPoint> m_currentLinks;
    bool m_changed;
};

inline
bool LinkPointManager::changed() const
{
    return m_changed;
}

inline
void LinkPointManager::slotItemChanged()
{
    m_changed = true;
}

#endif // LINKPOINTMANAGER_H
