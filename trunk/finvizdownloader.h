#ifndef FINVIZDOWNLOADER_H
#define FINVIZDOWNLOADER_H

#include <QStringList>
#include <QDialog>

class NetworkAccess;

class QUrl;

namespace Ui {
class FinvizDownloader;
}

class FinvizDownloader : public QDialog
{
    Q_OBJECT

public:
    explicit FinvizDownloader(const QUrl &url, QWidget *parent = 0);
    ~FinvizDownloader();

    QStringList tickers() const;

protected:
    virtual void closeEvent(QCloseEvent *);

private:
    void showError(const QString &);

private slots:
    void slotFinished();

private:
    Ui::FinvizDownloader *ui;
    NetworkAccess *m_net;
    QStringList m_tickers;
    bool m_allowClose;
};

inline
QStringList FinvizDownloader::tickers() const
{
    return m_tickers;
}

#endif // FINVIZDOWNLOADER_H
