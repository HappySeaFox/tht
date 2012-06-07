#ifndef TICKERNEIGHBORS_H
#define TICKERNEIGHBORS_H

#include <QStringList>
#include <QDialog>

namespace Ui {
class TickerNeighbors;
}

class QStandardItemModel;
class QCheckBox;

class TickerNeighbors : public QDialog
{
    Q_OBJECT

public:
    explicit TickerNeighbors(const QString &ticker, QWidget *parent = 0);
    ~TickerNeighbors();

private:
    void silentlyCheck(QCheckBox *, bool check);

private slots:
    void slotFilterAndFetch();
    void slotFetch();
    void slotCopy();

private:
    Ui::TickerNeighbors *ui;
    QStringList m_tickers;
    QStandardItemModel *m_model;
    QObject *m_lastAction;
};

#endif // TICKERNEIGHBORS_H
