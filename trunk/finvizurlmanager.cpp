#include <QUrl>

#include "finvizurlmanager.h"
#include "settings.h"
#include "ui_datamanagerbase.h"

FinvizUrlManager::FinvizUrlManager(QWidget *parent) :
    DataManagerBase(parent)
{
    setWindowTitle(tr("Finviz links"));
    ui->pushAdd->setText(tr("Add"));
    ui->tree->headerItem()->setText(1, tr("Link"));

    QList<FinvizUrl> urls = Settings::instance()->finvizUrls();

    foreach(FinvizUrl fu, urls)
    {
        addFinvizUrl(fu);
    }

    ui->tree->setCurrentItem(ui->tree->topLevelItem(0), QItemSelectionModel::ClearAndSelect);

    connect(ui->tree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(slotCheckItem(QTreeWidgetItem *, int)));
}

FinvizUrlManager::~FinvizUrlManager()
{
    delete ui;
}

QList<FinvizUrl> FinvizUrlManager::urls() const
{
    QList<FinvizUrl> urls;
    QTreeWidgetItem *i = ui->tree->topLevelItem(0);

    while(i)
    {
        urls.append(FinvizUrl(i->text(0), QUrl::fromUserInput(i->text(1))));
        i = ui->tree->itemBelow(i);
    }

    return urls;
}

void FinvizUrlManager::addFinvizUrl(const FinvizUrl &fu, bool edit)
{
    addItem(QStringList() << fu.name << fu.url.toString(), QVariant(), edit);
}

void FinvizUrlManager::slotAdd()
{
    addFinvizUrl(FinvizUrl(tr("Tickers"), tr("<Paste url here>")), true);
    m_changed = true;
}

void FinvizUrlManager::slotCheckItem(QTreeWidgetItem *i, int column)
{
    if(!i || column != 1)
        return;

    // check host
    QUrl u = i->text(1);

    if(u.host().toLower() != "finviz.com")
        i->setText(1, tr("<Paste url here>"));
}
