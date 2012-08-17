#include "finvizlinkselector.h"
#include "settings.h"
#include "ui_finvizlinkselector.h"

FinvizLinkSelector::FinvizLinkSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FinvizLinkSelector)
{
    ui->setupUi(this);

    m_urls = Settings::instance()->finvizUrls();

    foreach(FinvizUrl fu, m_urls)
    {
        ui->listWidget->addItem(fu.name);
    }

    ui->listWidget->setCurrentRow(0, QItemSelectionModel::ClearAndSelect);
}

FinvizLinkSelector::~FinvizLinkSelector()
{
    delete ui;
}

void FinvizLinkSelector::slotAdd()
{
    QString current = ui->listWidget->currentItem()->text();

    if(!current.isEmpty())
    {
        foreach(FinvizUrl fu, m_urls)
        {
            if(fu.name == current)
            {
                m_url = fu.url;
                break;
            }
        }
    }

    accept();
}
