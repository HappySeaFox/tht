#ifndef TICKERNEIGHBORS_H
#define TICKERNEIGHBORS_H

#include <QStringList>
#include <QDialog>

namespace Ui {
class TickerNeighbors;
}

class QStandardItemModel;

class TickerNeighbors : public QDialog
{
    Q_OBJECT

public:
    explicit TickerNeighbors(const QString &ticker, QWidget *parent = 0);
    ~TickerNeighbors();

private slots:
    void slotFetch();
    void slotCopy();

private:
    Ui::TickerNeighbors *ui;
    QStringList m_tickers;
    QStandardItemModel *m_model;
};

#endif // TICKERNEIGHBORS_H
