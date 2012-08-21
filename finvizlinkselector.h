#ifndef FINVIZLINKSELECTOR_H
#define FINVIZLINKSELECTOR_H

#include <QDialog>
#include <QList>
#include <QUrl>

class FinvizUrl;

namespace Ui
{
    class FinvizLinkSelector;
}

class FinvizLinkSelector : public QDialog
{
    Q_OBJECT

public:
    explicit FinvizLinkSelector(QWidget *parent = 0);
    ~FinvizLinkSelector();

    QUrl url() const;

private slots:
    void slotAdd();

private:
    Ui::FinvizLinkSelector *ui;
    QList<FinvizUrl> m_urls;
    QUrl m_url;
};

inline
QUrl FinvizLinkSelector::url() const
{
    return m_url;
}

#endif // FINVIZLINKSELECTOR_H
