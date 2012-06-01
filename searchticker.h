#ifndef SEARCHTICKER_H
#define SEARCHTICKER_H

#include <QWidget>

namespace Ui {
class SearchTicker;
}

class SearchTicker : public QWidget
{
    Q_OBJECT

public:
    explicit SearchTicker(QWidget *parent = 0);
    ~SearchTicker();

protected:
    virtual bool eventFilter(QObject *watched, QEvent *);

signals:
    void ticker(const QString &);

private:
    Ui::SearchTicker *ui;
};

#endif // SEARCHTICKER_H
