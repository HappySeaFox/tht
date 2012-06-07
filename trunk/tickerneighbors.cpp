#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QApplication>
#include <QClipboard>
#include <QTimer>

#include "tickerneighbors.h"
#include "uppercasevalidator.h"
#include "settings.h"
#include "sqltools.h"

#include "ui_tickerneighbors.h"

TickerNeighbors::TickerNeighbors(const QString &ticker, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TickerNeighbors)
{
    ui->setupUi(this);

    ui->lineTicker->setValidator(new UpperCaseValidator(ui->lineTicker));

    ui->lineTicker->setText(ticker);

    // fill sectors & industries
    qDebug("Getting sectors & industries");

    QStringList result = SqlTools::sectors();

    foreach(QString s, result)
    {
        ui->comboSector->addItem(s);
    }

    result = SqlTools::industries();

    foreach(QString i, result)
    {
        ui->comboIndustry->addItem(i);
    }

    // model for listview
    m_model = new QStandardItemModel(this);

    ui->list->setModel(m_model);

    // load checkboxes
    int nyse = Settings::instance()->checkBoxState(ui->checkNyse->text());
    int nasd = Settings::instance()->checkBoxState(ui->checkNasd->text());
    int amex = Settings::instance()->checkBoxState(ui->checkAmex->text());

    if(nyse <= 0 && nasd <= 0 && amex <= 0)
        silentlyCheck(ui->checkNyse, true);
    else
    {
        if(nyse >= 0)
            silentlyCheck(ui->checkNyse, nyse);

        if(nasd >= 0)
            silentlyCheck(ui->checkNasd, nasd);

        if(amex >= 0)
            silentlyCheck(ui->checkAmex, amex);
    }

    QTimer::singleShot(0, this, SLOT(slotFetch()));
}

TickerNeighbors::~TickerNeighbors()
{
    Settings::instance()->setCheckBoxState(ui->checkNyse->text(), ui->checkNyse->isChecked(), Settings::NoSync);
    Settings::instance()->setCheckBoxState(ui->checkNasd->text(), ui->checkNasd->isChecked(), Settings::NoSync);
    Settings::instance()->setCheckBoxState(ui->checkAmex->text(), ui->checkAmex->isChecked());

    delete ui;
}

void TickerNeighbors::silentlyCheck(QCheckBox *box, bool check)
{
    if(box)
    {
        bool b = box->signalsBlocked();
        box->blockSignals(true);
        box->setChecked(check);
        box->blockSignals(b);
    }
}

void TickerNeighbors::slotFetch()
{
    // clear old data
    m_model->clear();
    m_tickers.clear();

    ui->labelFound->setNum(0);

    QList<QStringList> lists;
    QMap<QString, QString> binds;
    QString sector, industry;

    QString ticker = ui->lineTicker->text().replace('.', '-');

    qDebug("Getting neighbors for ticker \"%s\"", qPrintable(ticker));

    // ticker info
    if(sender() != ui->pushSector && sender() != ui->pushIndustry)
    {
        lists = SqlTools::query("SELECT sector, industry FROM tickers WHERE ticker = :ticker", ":ticker", ticker);

        if(lists.isEmpty())
            return;

        QStringList list = lists.at(0);

        if(list.size() != 2)
            return;

        sector = list.at(0);
        industry = list.at(1);

        if(ui->comboSector->currentText() != sector)
            ui->comboSector->setCurrentIndex(ui->comboSector->findText(sector));

        if(ui->comboIndustry->currentText() != industry)
            ui->comboIndustry->setCurrentIndex(ui->comboIndustry->findText(industry));
    }
    else
    {
        sector = ui->comboSector->currentText();
        industry = ui->comboIndustry->currentText();
    }

    qDebug("Found neighbors info %s / %s", qPrintable(sector), qPrintable(industry));

    // sector & industry
    QList<QStringList> tickers;
    QString add;

    binds.clear();

    // exchanges
    if(ui->checkNyse->isChecked() || ui->checkNasd->isChecked() || ui->checkAmex->isChecked())
    {
        add = " AND ( ";
        bool secondQuery = false;

        if(ui->checkNyse->isChecked())
        {
            add += "exchange = :exchange1";
            binds.insert(":exchange1", "NYSE");
            secondQuery = true;
        }

        if(ui->checkNasd->isChecked())
        {
            if(secondQuery)
                add += " OR ";

            add += "exchange = :exchange2";
            binds.insert(":exchange2", "NASD");
            secondQuery = true;
        }

        if(ui->checkAmex->isChecked())
        {
            if(secondQuery)
                add += " OR ";

            add += "exchange = :exchange3";
            binds.insert(":exchange3", "AMEX");
        }

        add += " )";

        // final query
        if(sender() == ui->pushSector)
        {
            binds.insert(":sector", sector);
            tickers = SqlTools::query("SELECT ticker FROM tickers WHERE sector = :sector" + add, binds);
        }
        else
        {
            binds.insert(":industry", industry);
            tickers = SqlTools::query("SELECT ticker FROM tickers WHERE industry = :industry" + add, binds);
        }

        foreach(QStringList l, tickers)
        {
            if(l.size())
                m_tickers.append(l.at(0));
        }
    }

    ui->labelFound->setNum(m_tickers.size());

    qDebug("Found neighbors %d", m_tickers.size());

    foreach(QString t, m_tickers)
    {
        QStandardItem *item = new QStandardItem(t.replace('-', '.'));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        m_model->appendRow(item);
    }
}

void TickerNeighbors::slotCopy()
{
    QString text;
    QModelIndexList selected = ui->list->selectionModel()->selectedIndexes();

    if(selected.isEmpty())
        text = m_tickers.join("\n");
    else
    {
        foreach(QModelIndex index, selected)
        {
            text += index.data().toString() + '\n';
        }

        text.chop(1);
    }

    QApplication::clipboard()->setText(text);
}
