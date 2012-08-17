#include <QMutableListIterator>
#include <QCloseEvent>
#include <QByteArray>
#include <QDebug>
#include <QUrl>

#include "finvizdownloader.h"
#include "networkaccess.h"
#include "csvreader.h"
#include "ui_finvizdownloader.h"

static const int REQUIRED_FIELDS = 11;

FinvizDownloader::FinvizDownloader(const QUrl &url, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FinvizDownloader),
    m_allowClose(false)
{
    ui->setupUi(this);

    ui->pushClose->hide();

    m_net = new NetworkAccess(this);

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));

    QUrl u = url;

    // fix path
    u.setPath(u.path().replace("screener.ashx", "export.ashx"));

    QList<QPair<QString, QString> > items = u.queryItems();

    QMutableListIterator<QPair<QString, QString> > i(items);

    // fix "v" query item
    while(i.hasNext())
    {
        QPair<QString, QString> pair = i.next();

        if(pair.first == "v")
        {
            if(pair.second != "151")
            {
                pair.second = "151";
                i.setValue(pair);
                u.setQueryItems(items);
            }

            break;
        }
    }

    // download tickers as CSV
    m_net->get(u);
}

FinvizDownloader::~FinvizDownloader()
{
    delete ui;
}

void FinvizDownloader::closeEvent(QCloseEvent *e)
{
    if(m_allowClose)
        e->accept();
    else
        e->ignore();
}

void FinvizDownloader::showError(const QString &e)
{
    ui->label->setText(e);
    ui->pushClose->show();
    ui->progressBar->setRange(0, 1);
    ui->progressBar->setValue(1);
    m_allowClose = true;
}

void FinvizDownloader::slotFinished()
{
    if(m_net->error() != QNetworkReply::NoError)
    {
        showError(tr("Network error #%1").arg(m_net->error()));
        return;
    }

    // parse CSV
    CsvParser csv(m_net->data());

    QStringList str;
    csv.parseLine();

    while(!(str = csv.parseLine()).isEmpty())
    {
        if(str.size() != REQUIRED_FIELDS)
        {
            showError(tr("Broken data (fields: %1, required: %2)").arg(str.size()).arg(REQUIRED_FIELDS));
            return;
        }

        m_tickers.append(str[1]);
    }

    accept();
}
