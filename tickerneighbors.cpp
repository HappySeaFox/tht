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
#include <QApplication>
#include <QMetaObject>
#include <QModelIndex>
#include <QClipboard>
#include <QShortcut>
#include <QKeyEvent>
#include <QCheckBox>
#include <QEvent>
#include <QTimer>

#include "tickerneighbors.h"
#include "uppercasevalidator.h"
#include "settings.h"
#include "sqltools.h"
#include "tools.h"

#include "ui_tickerneighbors.h"

TickerNeighbors::TickerNeighbors(const QString &ticker, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TickerNeighbors)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    // Ctrl+C to copy
    m_copy = QKeySequence::Copy;
    new QShortcut(m_copy, ui->pushCopy, SLOT(animateClick()));

    // install event filter on all lineedits, because they steal keyboard
    // focus and block window shortcuts
    QList<QLineEdit *> list = findChildren<QLineEdit *>();

    foreach(QLineEdit *edit, list)
    {
        edit->installEventFilter(this);
    }

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

    // fill exchanges
    QHBoxLayout *le = new QHBoxLayout;
    le->setContentsMargins(0, 0, 0, 0);
    ui->widgetExchanges->setLayout(le);

    result = SqlTools::exchanges();
    int anystate = 0;

    foreach(QString e, result)
    {
        QCheckBox *box = new QCheckBox(e, ui->widgetExchanges);

        box->setObjectName(e);

        connect(box, SIGNAL(toggled(bool)), this, SLOT(slotCheckboxChanged()));

        le->addWidget(box);
        m_exchanges.append(box);

        int state = Settings::instance()->checkBoxState(e);

        if(state >= 0)
        {
            silentlyCheck(box, state);
            anystate++;
        }
    }

    // no exchanges selected at all, select all of them
    if(!anystate)
    {
        foreach(QCheckBox *box, m_exchanges)
        {
            silentlyCheck(box, 1);
            Settings::instance()->setCheckBoxState(box->objectName(), box->isChecked(), Settings::NoSync);
        }
    }

    if(Settings::instance()->checkBoxState(ui->checkCap->objectName()) > 0)
            silentlyCheck(ui->checkCap, true);

    if(Settings::instance()->checkBoxState(ui->checkUSA->objectName()) > 0)
            silentlyCheck(ui->checkUSA, true);

    connect(ui->listTickers->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(slotSelectionChanged()));

    if(Settings::instance()->saveGeometry())
    {
        m_pos = Settings::instance()->neighborsWindowPosition();

        QSize sz = Settings::instance()->neighborsWindowSize();

        if(sz.isValid())
            resize(sz);
    }

    showTicker(ticker);
}

TickerNeighbors::~TickerNeighbors()
{
    if(Settings::instance()->saveGeometry())
    {
        Settings::instance()->setNeighborsWindowSize(size(), Settings::NoSync);
        Settings::instance()->setNeighborsWindowPosition(pos(), Settings::Sync);
    }

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
    else if(!isVisible())
        Tools::moveWindow(this, m_pos);

    QDialog::setVisible(vis);
}

bool TickerNeighbors::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)

    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);

        if(ke && ke->matches(m_copy) && !ui->lineTicker->hasSelectedText())
        {
            ui->pushCopy->animateClick();
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
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

void TickerNeighbors::filterAndFetch()
{
    // emulate click
    QMetaObject::invokeMethod(m_lastAction, "clicked");
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

    add = " AND ( ";
    bool secondQuery = false;
    int i = 0;

    // exchanges
    foreach(QCheckBox *box, m_exchanges)
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

    // USA
    if(ui->checkUSA->isChecked())
    {
        add += " AND country = :country";
        binds.insert(":country", "USA");
    }

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
    qDebug("Copy");

    QString text;
    const QModelIndexList selected = ui->listTickers->selectionModel()->selectedIndexes();

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

void TickerNeighbors::slotCheckboxChanged()
{
    QCheckBox *box = qobject_cast<QCheckBox *>(sender());

    if(!box)
        return;

    Settings::instance()->setCheckBoxState(box->objectName(), box->isChecked(), Settings::NoSync);

    filterAndFetch();
}
