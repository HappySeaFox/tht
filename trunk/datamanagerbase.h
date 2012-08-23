#ifndef DATAMANAGERBASE_H
#define DATAMANAGERBASE_H

#include <QDialog>

class QStringList;
class QVariant;

namespace Ui
{
    class DataManagerBase;
}

class DataManagerBase : public QDialog
{
    Q_OBJECT

public:
    explicit DataManagerBase(QWidget *parent = 0);
    virtual ~DataManagerBase();

    bool changed() const;

protected:
    virtual bool event(QEvent *);
    void addItem(const QStringList &strings, const QVariant &data, bool edit = false);

private:
    void moveItem(int index, int diff);

private slots:
    virtual void slotAdd() = 0;
    void slotDelete();
    void slotUp();
    void slotDown();
    void slotClear();
    void slotItemChanged();

protected:
    Ui::DataManagerBase *ui;
    bool m_changed;
};

inline
bool DataManagerBase::changed() const
{
    return m_changed;
}

inline
void DataManagerBase::slotItemChanged()
{
    m_changed = true;
}

#endif // DATAMANAGERBASE_H
