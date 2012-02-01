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

#include <QListWidgetItem>
#include <QFileDialog>
#include <QInputEvent>
#include <QMessageBox>
#include <QTextStream>
#include <QClipboard>
#include <QPalette>
#include <QKeyEvent>
#include <QEvent>
#include <QMenu>

#include "settings.h"
#include "list.h"
#include "ui_list.h"

List::List(int group, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::List),
    m_section(group),
    m_saveTickers(Settings::instance()->saveTickers())
{
    ui->setupUi(this);

    ui->pushSave->setEnabled(!m_saveTickers);

    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Load from file..."), this, SLOT(slotAddFromFile()));
    menu->addAction(tr("Load from clipboard"), this, SLOT(slotAddFromClipboard()));
    ui->pushAdd->setMenu(menu);

    menu = new QMenu(this);
    menu->addAction(tr("Export to file..."), this, SLOT(slotExportToFile()));
    menu->addAction(tr("Export to clipboard"), this, SLOT(slotExportToClipboard()));
    ui->pushSaveAs->setMenu(menu);

    setFocusProxy(ui->list);
    ui->pushAdd->setFocusProxy(ui->list);
    ui->pushClear->setFocusProxy(ui->list);
    ui->pushSaveAs->setFocusProxy(ui->list);

    load();

    // catch keyboard events
    ui->list->installEventFilter(this);
    ui->list->viewport()->installEventFilter(this);

    // regexp to check for a ticker
    m_rxTicker = QRegExp("$?[a-zA-Z\\-]{1,5}");
}

List::~List()
{
    delete ui;
}

void List::addTicker(const QString &ticker)
{
    if(ui->list->findItems(ticker, Qt::MatchFixedString).size())
    {
        qDebug("THT: Will not add duplicate item");
        return;
    }

    if(m_rxTicker.exactMatch(ticker))
    {
        ui->list->addItem(ticker);
        numberOfItemsChanged();
        save();
    }
}

QString List::currentTicker() const
{
    QListWidgetItem *item = ui->list->currentItem();

    return item ? item->text() : QString();
}

void List::setSaveTickers(bool dosave)
{
    if(m_saveTickers == dosave)
        return;

    bool oldsave = m_saveTickers;
    m_saveTickers = dosave;
    ui->pushSave->setEnabled(!m_saveTickers);

    if(!oldsave)
        save();
}

bool List::eventFilter(QObject *obj, QEvent *event)
{
    QEvent::Type type = event->type();

    // eat input events
    if(m_ignoreInput && (type == QEvent::KeyPress
                            || type == QEvent::KeyRelease
                            || type == QEvent::ShortcutOverride
                            || type == QEvent::MouseButtonPress
                            || type == QEvent::MouseButtonRelease
                            || type == QEvent::MouseButtonDblClick
                            || type == QEvent::MouseMove
                            || type == QEvent::Wheel
                            || type == QEvent::TabletPress
                            || type == QEvent::TabletRelease
                            || type == QEvent::TabletMove
                            || type == QEvent::TouchBegin
                            || type == QEvent::TouchEnd
                            || type == QEvent::TouchUpdate
                         ))
        return true;

    if(type == QEvent::KeyPress)
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);

        if(ke->matches(QKeySequence::Paste))
            paste();
        else if(ke->matches(QKeySequence::New))
            clear();
        else if(ke->key() == Qt::Key_Right)
            emit moveRight(currentTicker());
        else if(ke->key() == Qt::Key_Left)
            emit moveLeft(currentTicker());
        else if(ke->key() == Qt::Key_Delete)
        {
            delete ui->list->currentItem();
            numberOfItemsChanged();
            save();
        }
        else if(ke->key() == Qt::Key_Return)
        {
            QListWidgetItem *item = ui->list->currentItem();

            if(!item)
                item = ui->list->item(0);

            if(item)
            {
                if(item->isSelected())
                    slotSelectedItemChanged();
                else
                {
                    ui->list->setCurrentItem(item);
                    item->setSelected(true);
                }
            }
        }

        return QObject::eventFilter(obj, event);
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }
}

void List::numberOfItemsChanged()
{
    ui->labelElements->setNum(ui->list->count());
}

QStringList List::toStringList()
{
    int i = 0;
    QStringList items;
    QListWidgetItem *item;

    while((item = ui->list->item(i++)))
    {
        items.append(item->text());
    }

    return items;
}

void List::save()
{
    // don't save tickers automatically
    if(!m_saveTickers)
    {
        qDebug("THT: Autosave is disabled");
        showSaved(false);
        return;
    }

    slotSave();
}

void List::load()
{
    qDebug("THT: Loading tickers from section \"%d\"", m_section);

    ui->list->addItems(Settings::instance()->tickersForGroup(m_section));

    numberOfItemsChanged();
}

void List::paste()
{
    QString text = QApplication::clipboard()->text();

    qDebug("THT: Paste");

    QTextStream t(&text);
    QString ticker;

    ui->list->setUpdatesEnabled(false);

    bool changed = false;

    while(!t.atEnd())
    {
        t >> ticker;

        if(m_rxTicker.exactMatch(ticker))
        {
            changed = true;
            ui->list->addItem(ticker.toUpper());
        }
    }

    ui->list->setUpdatesEnabled(true);

    if(changed)
    {
        numberOfItemsChanged();
        save();
    }
}

void List::showSaved(bool isSaved)
{
    QPalette pal = ui->labelUnsaved->palette();
    pal.setColor(QPalette::Window, isSaved ? palette().color(QPalette::Window) : Qt::red);
    ui->labelUnsaved->setPalette(pal);
}

void List::slotAddFromFile()
{
    qDebug("THT: Adding new tickers from file");

    QString fileName = QFileDialog::getOpenFileName(this, tr("Save as"));

    if(fileName.isEmpty())
        return;

    QString ticker;
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly))
    {
        qWarning("THT: Cannot open file for writing");
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file %1").arg(fileName));
        return;
    }

    bool changed = false;
    QTextStream t(&file);

    ui->list->setUpdatesEnabled(false);

    while(!t.atEnd())
    {
        t >> ticker;

        if(m_rxTicker.exactMatch(ticker))
        {
            changed = true;
            ui->list->addItem(ticker.toUpper());
        }
    }

    ui->list->setUpdatesEnabled(true);

    if(changed)
    {
        numberOfItemsChanged();
        save();
    }
}

void List::slotAddFromClipboard()
{
    paste();
}

void List::clear()
{
    qDebug("THT: Clear tickers for section \"%d\"", m_section);

    // nothing to do
    if(!ui->list->count())
        return;

    ui->list->clear();
    numberOfItemsChanged();
    save();
}

void List::slotSave()
{
    qDebug("THT: Saving tickers to section \"%d\"", m_section);

    Settings::instance()->saveTickersForGroup(m_section, toStringList());

    showSaved(true);
}

void List::slotExportToFile()
{
    qDebug("THT: Exporting tickers to file");

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"));

    if(!fileName.isEmpty())
    {
        QFile file(fileName);

        if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            qWarning("THT: Cannot open file for writing");
            QMessageBox::warning(this, tr("Error"), tr("Cannot save to file %1").arg(fileName));
            return;
        }

        QTextStream t(&file);

        QStringList items = toStringList();

        foreach(QString item, items)
        {
            t << item << '\n';
        }

        t.flush();
    }
}

void List::slotExportToClipboard()
{
    qDebug("THT: Exporting tickers to clipboard");

    QApplication::clipboard()->setText(toStringList().join("\n"));
}

void List::slotSelectedItemChanged()
{
    QList<QListWidgetItem *> selected = ui->list->selectedItems();

    // load selected ticker
    if(selected.size())
    {
        QListWidgetItem *first = selected.at(0);

        if(first && first == ui->list->currentItem())
            emit loadTicker(first->text());
    }
}

