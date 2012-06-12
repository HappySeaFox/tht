#ifndef TICKERNEIGHBORS_H
#define TICKERNEIGHBORS_H

#include <QStringList>
#include <QDialog>

namespace Ui {
class TickerNeighbors;
}

class QModelIndex;
class QCheckBox;

class TickerNeighbors : public QDialog
{
    Q_OBJECT

public:
    explicit TickerNeighbors(const QString &ticker, QWidget *parent = 0);
    ~TickerNeighbors();

    void showTicker(const QString &);

private:
    void silentlyCheck(QCheckBox *, bool check);

signals:
    void loadTicker(const QString &);

private slots:
    void slotFilterAndFetch();
    void slotFetch();
    void slotCopy();
    void slotSelectionChanged();
    void slotActivated(const QModelIndex &);
private:
    Ui::TickerNeighbors *ui;
    QStringList m_tickers;
    QObject *m_lastAction;
};

#endif // TICKERNEIGHBORS_H
