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

#include "tickerinformationtooltip.h"
#include "uppercasevalidator.h"
#include "tickerneighbors.h"
#include "thtsettings.h"
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

    // setup context menus for combos
    QAction *a;

    //: This is the label on a menu item that user clicks to issue the command
    a = new QAction(tr("Copy"), ui->comboSector);
    connect(a, SIGNAL(triggered()), this, SLOT(slotCopyCombo()));
    ui->comboSector->addAction(a);

    a = new QAction(tr("Copy"), ui->comboIndustry);
    connect(a, SIGNAL(triggered()), this, SLOT(slotCopyCombo()));
    ui->comboIndustry->addAction(a);

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

    resetTabOrders();

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

    if(SETTINGS_GET_BOOL(SETTING_SAVE_GEOMETRY))
    {
        m_pos = SETTINGS_GET_POINT(SETTING_NEIGHBORS_POSITION);

        QSize sz = SETTINGS_GET_SIZE(SETTING_NEIGHBORS_SIZE);

        if(sz.isValid())
            resize(sz);
    }
    else
        m_pos = Tools::invalidQPoint;

    showTicker(ticker);

    ui->listTickers->installEventFilter(this);
    ui->lineTicker->installEventFilter(this);
}

TickerNeighbors::~TickerNeighbors()
{
    if(SETTINGS_GET_BOOL(SETTING_SAVE_GEOMETRY))
    {
        SETTINGS_SET_SIZE(SETTING_NEIGHBORS_SIZE, size(), Settings::NoSync);
        SETTINGS_SET_POINT(SETTING_NEIGHBORS_POSITION, pos());
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

    QEvent::Type type = event->type();

    if(qobject_cast<QLineEdit *>(obj))
    {
        if(type == QEvent::KeyPress)
        {
            QKeyEvent *ke = static_cast<QKeyEvent *>(event);

            if(ke)
            {
                // check for "copy tickers" shortcut
                if(ke->matches(m_copy) && !ui->lineTicker->hasSelectedText())
                {
                    ui->pushCopy->animateClick();
                    return true;
                }
                else if(obj == ui->lineTicker)
                {
                    if(ke->key() == Qt::Key_Space)
                    {
                        TickerInformationToolTip::showText(ui->lineTicker->mapToGlobal(QPoint(0, ui->lineTicker->height())),
                                                           ui->lineTicker->text());
                        return true;
                    }
                }
            }
        }
    }
    else if(obj == ui->listTickers)
    {
        if(type == QEvent::KeyPress)
        {
            QKeyEvent *ke = static_cast<QKeyEvent *>(event);

            if(ke && ke->key() == Qt::Key_Space)
            {
                QListWidgetItem *item = ui->listTickers->currentItem();
                QRect rc = ui->listTickers->visualItemRect(item);

                if(item && rc.isValid())
                {
                    TickerInformationToolTip::showText(ui->listTickers->viewport()->mapToGlobal(rc.bottomLeft()),
                                                       item->text());
                    return true;
                }
            }
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

void TickerNeighbors::resetTabOrders()
{
    QWidget *lastWidget = ui->listTickers;

    foreach(QCheckBox *box, m_exchanges)
    {
        QWidget::setTabOrder(lastWidget, box);
        lastWidget = box;
    }

    QWidget::setTabOrder(lastWidget, ui->checkCap);
    QWidget::setTabOrder(ui->checkCap, ui->checkUSA);
    QWidget::setTabOrder(ui->checkUSA, ui->pushCopy);
}

void TickerNeighbors::showNumberOfFilesToCopy(int n)
{
    ui->pushCopy->setText(QString("%1 (%2)").arg(tr("Copy")).arg(n));
}

void TickerNeighbors::slotFetch()
{
    // clear old data
    ui->listTickers->clear();
    m_tickers.clear();

    showNumberOfFilesToCopy(0);

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

        QVariantList list;

        if(lists.isEmpty() || (list = lists.at(0)).size() != 2)
        {
            ui->comboSector->setCurrentIndex(-1);
            ui->comboIndustry->setCurrentIndex(-1);
            return;
        }

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

    showNumberOfFilesToCopy(m_tickers.size());

    qDebug("Found neighbors: %d", m_tickers.size());
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

    showNumberOfFilesToCopy(count ? count : ui->listTickers->count());
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

void TickerNeighbors::slotCopyCombo()
{
    QComboBox *box = qobject_cast<QComboBox *>(sender()->parent());

    if(!box)
        return;

    QApplication::clipboard()->setText(box->currentText());
}
