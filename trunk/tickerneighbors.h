#ifndef TICKERNEIGHBORS_H
#define TICKERNEIGHBORS_H

#include <QStringList>
#include <QDialog>
#include <QPoint>

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

    virtual void setVisible(bool);

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
    QPoint m_pos;
};

#endif // TICKERNEIGHBORS_H
