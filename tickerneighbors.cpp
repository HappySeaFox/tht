/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QItemSelectionModel>
#include <QTreeWidgetItem>
#include <QDesktopWidget>
#include <QApplication>
#include <QMetaObject>
#include <QModelIndex>
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
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    ui->lineTicker->setValidator(new UpperCaseValidator(ui->lineTicker));

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

    // load checkboxes
    int nyse = Settings::instance()->checkBoxState(ui->checkNyse->objectName());
    int nasd = Settings::instance()->checkBoxState(ui->checkNasd->objectName());
    int amex = Settings::instance()->checkBoxState(ui->checkAmex->objectName());

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

    if(Settings::instance()->checkBoxState(ui->checkCap->objectName()) > 0)
            silentlyCheck(ui->checkCap, true);

    connect(ui->listTickers->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(slotSelectionChanged()));

    showTicker(ticker);
}

TickerNeighbors::~TickerNeighbors()
{
    Settings::instance()->setCheckBoxState(ui->checkNyse->objectName(), ui->checkNyse->isChecked(), Settings::NoSync);
    Settings::instance()->setCheckBoxState(ui->checkNasd->objectName(), ui->checkNasd->isChecked(), Settings::NoSync);
    Settings::instance()->setCheckBoxState(ui->checkAmex->objectName(), ui->checkAmex->isChecked(), Settings::NoSync);
    Settings::instance()->setCheckBoxState(ui->checkCap->objectName(), ui->checkCap->isChecked());

    delete ui;
}

void TickerNeighbors::showTicker(const QString &ticker)
{
    ui->lineTicker->setText(ticker);
    m_lastAction = ui->pushTicker;
    QTimer::singleShot(0, this, SLOT(slotFetch()));
}

void TickerNeighbors::setVisible(bool vis)
{
    if(!vis)
        m_pos = pos();

    QDialog::setVisible(vis);

    if(vis && !m_pos.isNull())
    {
        QRect dr = qApp->desktop()->availableGeometry();
        QRect headGeometry = QRect(m_pos, QSize(width(), 20));

        // move to a valid position
        if(dr.contains(headGeometry.topLeft()) || dr.contains(headGeometry.bottomRight()))
            move(m_pos);
    }
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
    ui->listTickers->clear();
    m_tickers.clear();

    ui->pushCopy->setText(tr("Copy (%1)").arg(0));

    QList<QVariantList> lists;
    QMap<QString, QString> binds;
    QString sector, industry;

    QString ticker = ui->lineTicker->text().replace('.', '-');

    qDebug("Getting neighbors for ticker \"%s\"", qPrintable(ticker));

    QObject *osender = sender();

    // save last action object
    if(osender == ui->pushTicker || osender == ui->pushSector || osender == ui->pushIndustry)
        m_lastAction = osender;

    // ticker info
    if(osender != ui->pushSector && osender != ui->pushIndustry)
    {
        lists = SqlTools::query("SELECT sector, industry FROM tickers WHERE ticker = :ticker", ":ticker", ticker);

        if(lists.isEmpty())
            return;

        QVariantList list = lists.at(0);

        if(list.size() != 2)
            return;

        sector = list.at(0).toString();
        industry = list.at(1).toString();

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
    QList<QVariantList> tickers;
    QString add;

    binds.clear();

    // exchanges
    QList<QCheckBox *> exchanges = QList<QCheckBox *>() << ui->checkNyse << ui->checkNasd << ui->checkAmex;

    add = " AND ( ";
    bool secondQuery = false;
    int i = 0;

    foreach(QCheckBox *box, exchanges)
    {
        if(box->isChecked())
        {
            if(secondQuery)
                add += " OR ";

            QString ind = QString::number(i++);
            add += "exchange = :exchange" + ind;
            binds.insert(":exchange" + ind, box->text());
            secondQuery = true;
        }
    }
    add += " )";

    // no exchanges
    if(!secondQuery)
        return;

    if(ui->checkCap->isChecked())
        add += " ORDER BY cap DESC";
    else
        add += " ORDER BY ticker ASC";

    // final query
    if(osender == ui->pushSector)
    {
        binds.insert(":sector", sector);
        tickers = SqlTools::query("SELECT ticker FROM tickers WHERE sector = :sector" + add, binds);
    }
    else
    {
        binds.insert(":industry", industry);
        tickers = SqlTools::query("SELECT ticker FROM tickers WHERE industry = :industry" + add, binds);
    }

    foreach(QVariantList l, tickers)
    {
        if(l.isEmpty())
            continue;

        add = l[0].toString().replace('-', '.');

        m_tickers.append(add);

        QListWidgetItem *item = new QListWidgetItem(add, ui->listTickers);

        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }

    ui->pushCopy->setText(tr("Copy (%1)").arg(m_tickers.size()));

    qDebug("Found neighbors %d", m_tickers.size());
}

void TickerNeighbors::slotCopy()
{
    QString text;
    QModelIndexList selected = ui->listTickers->selectionModel()->selectedIndexes();

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

void TickerNeighbors::slotFilterAndFetch()
{
    QMetaObject::invokeMethod(m_lastAction, "clicked");
}

void TickerNeighbors::slotSelectionChanged()
{
    const int count = ui->listTickers->selectionModel()->selectedRows().count();

    ui->pushCopy->setText(tr("Copy (%1)").arg(!count ? ui->listTickers->count() : count));
}

void TickerNeighbors::slotActivated(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    emit loadTicker((index.column() ? index.sibling(index.row(), 0) : index).data().toString());
}
