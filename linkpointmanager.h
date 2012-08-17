#ifndef LINKPOINTMANAGER_H
#define LINKPOINTMANAGER_H

#include <QPoint>
#include <QList>

#include "datamanagerbase.h"
#include "settings.h"

class LinkPointManager : public DataManagerBase
{
    Q_OBJECT

public:
    explicit LinkPointManager(const QList<QPoint> &currentLinks, QWidget *parent = 0);
    ~LinkPointManager();

    QList<LinkPoint> links() const;

private:
    void addLinkPoint(const LinkPoint &, bool edit = false);

private slots:
    virtual void slotAdd();

private:
    QList<QPoint> m_currentLinks;
};

#endif // LINKPOINTMANAGER_H
