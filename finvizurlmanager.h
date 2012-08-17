#ifndef FINVIZURLMANAGER_H
#define FINVIZURLMANAGER_H

#include <QList>

#include "datamanagerbase.h"

class FinvizUrl;

class FinvizUrlManager : public DataManagerBase
{
    Q_OBJECT
    
public:
    explicit FinvizUrlManager(QWidget *parent = 0);
    ~FinvizUrlManager();

    QList<FinvizUrl> urls() const;

private:
    void addFinvizUrl(const FinvizUrl &fu, bool edit = false);

private slots:
    virtual void slotAdd();
};

#endif // FINVIZURLMANAGER_H
