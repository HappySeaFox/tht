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

#include <QDesktopServices>
#include <QListWidgetItem>
#include <QLinearGradient>
#include <QApplication>
#include <QFontMetrics>
#include <QFileDialog>
#include <QInputEvent>
#include <QMessageBox>
#include <QTextStream>
#include <QMouseEvent>
#include <QClipboard>
#include <QGradient>
#include <QFileInfo>
#include <QKeyEvent>
#include <QPalette>
#include <QPainter>
#include <QEvent>
#include <QColor>
#include <QMenu>
#include <QSize>
#include <QUrl>
#include <QPen>

#include "tickerinformationtooltip.h"
#include "tickerinput.h"
#include "settings.h"
#include "list.h"
#include "ui_list.h"

List::List(int group, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::List),
    m_section(group),
    m_saveTickers(Settings::instance()->saveTickers()),
    m_ignoreInput(false),
    m_dragging(false)
{
    ui->setupUi(this);

    ui->pushSave->setEnabled(!m_saveTickers);

    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Add one ticker..."), this, SLOT(slotAddOne()));
    menu->addAction(tr("Add from file..."), this, SLOT(slotAddFromFile()));
    menu->addAction(tr("Add from clipboard"), this, SLOT(slotAddFromClipboard()));
    ui->pushAdd->setMenu(menu);

    menu = new QMenu(this);
    menu->addAction(tr("Export to file..."), this, SLOT(slotExportToFile()));
    menu->addAction(tr("Export to clipboard"), this, SLOT(slotExportToClipboard()));
    ui->pushSaveAs->setMenu(menu);

    setFocusProxy(ui->list);

    load();

    // catch keyboard events
    ui->list->installEventFilter(this);
    ui->list->viewport()->installEventFilter(this);
}

List::~List()
{
    delete ui;
}

bool List::haveTickers() const
{
    return ui->list->count();
}

void List::addTicker(const QString &ticker)
{
    if(ui->list->findItems(ticker, Qt::MatchFixedString).size())
    {
        qDebug("THT: Will not add duplicate item");
        return;
    }

    if(Settings::instance()->tickerValidator().exactMatch(ticker))
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
    if(m_ignoreInput && obj == ui->list &&
                            (type == QEvent::KeyPress
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

    if(obj == ui->list)
    {
        if(type == QEvent::KeyPress)
        {
            QKeyEvent *ke = static_cast<QKeyEvent *>(event);

            if(ke->matches(QKeySequence::Paste))
                paste();
            else if(ke->matches(QKeySequence::New))
                clear();
            else if(ke->matches(QKeySequence::Open))
                slotAddFromFile();
            else
            {
                switch(ke->key())
                {
                case Qt::Key_1:
                case Qt::Key_2:
                case Qt::Key_3:
                case Qt::Key_4:
                case Qt::Key_5:
                    emit copyTo(currentTicker(), ke->key() - Qt::Key_1);
                break;

                case Qt::Key_A:
                    slotAddFromFile();
                break;

                case Qt::Key_C:
                    paste();
                break;

                case Qt::Key_D:
                    slotExportToClipboard();
                break;

                case Qt::Key_E:
                    slotExportToFile();
                break;

                case Qt::Key_O:
                    slotAddOne();
                break;

                case Qt::Key_Right:
                    emit copyRight(currentTicker());
                break;

                case Qt::Key_Left:
                    emit copyLeft(currentTicker());
                break;

                case Qt::Key_Delete:
                {
                    QListWidgetItem *i = ui->list->currentItem();

                    if(i)
                    {
                        delete i;
                        numberOfItemsChanged();
                        save();
                        loadItem(LoadItemCurrent);
                    }
                }
                break;

                case Qt::Key_Return:
                    loadItem(LoadItemCurrent);
                break;

                case Qt::Key_N:
                    clear();
                break;

                // Yahoo finance
                case Qt::Key_Y:
                {
                    QString t = currentTicker();

                    if(!t.isEmpty())
                        QDesktopServices::openUrl(QUrl(QString("http://finance.yahoo.com/q?s=%1").arg(t.replace('.', '-'))));

                    break;
                }

                // $INDU
                case Qt::Key_I:
                    emit loadTicker("$INDU");
                break;

                // $SPX
                case Qt::Key_S:
                    emit loadTicker("$SPX");
                break;

                // $TVOL
                case Qt::Key_T:
                    emit loadTicker("$TVOL");
                break;

                // $VIX
                case Qt::Key_V:
                    emit loadTicker("$VIX");
                break;

                // $XOI
                case Qt::Key_X:
                    emit loadTicker("$XOI");
                break;

                // $HUI
                case Qt::Key_H:
                    emit loadTicker("$HUI");
                break;

                // default processing
                case Qt::Key_Up:
                    loadItem(LoadItemPrevious);
                break;

                case Qt::Key_Down:
                    loadItem(LoadItemNext);
                break;

                case Qt::Key_Home:
                    loadItem(LoadItemFirst);
                break;

                case Qt::Key_End:
                    loadItem(LoadItemLast);
                break;

                case Qt::Key_Space:
                    showTickerInfo();
                break;

                case Qt::Key_PageUp:
                case Qt::Key_PageDown:
                break;

                case Qt::Key_Tab:
                    return QObject::eventFilter(obj, event);
                } // switch
            }

            return true;
        }
        else if(type == QEvent::FocusIn)
            ui->list->setAlternatingRowColors(true);
        else if(type == QEvent::FocusOut)
            ui->list->setAlternatingRowColors(false);
    }
    else if(obj == ui->list->viewport())
    {
        if(type == QEvent::MouseButtonPress)
        {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);

            if(me->buttons() & Qt::LeftButton)
            {
                m_startPos = me->pos();

                QListWidgetItem *i = ui->list->itemAt(m_startPos);
                m_startDragText = i ? i->text() : QString();

                if(!m_startDragText.isEmpty())
                    m_startPos = me->pos();
            }
        }
        else if(type == QEvent::MouseMove)
        {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);

            if(!m_dragging)
            {
                if(!m_startPos.isNull()
                        && (me->buttons() & Qt::LeftButton)
                        && (me->pos() - m_startPos).manhattanLength() > QApplication::startDragDistance())
                {
                    qDebug("THT: Start dragging \"%s\"", qPrintable(m_startDragText));

                    QPixmap pix = createDragCursor();

                    m_dragging = true;
                    QApplication::setOverrideCursor(QCursor(pix, pix.width()/2, pix.height()/2));
                }
            }
            else
            {
                if(me->buttons() == Qt::NoButton)
                {
                    QApplication::restoreOverrideCursor();
                    m_dragging = false;
                    m_startPos = QPoint();
                }
            }

            return true;
        }
        else if(type == QEvent::MouseButtonRelease)
        {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);

            if(me->button() == Qt::LeftButton || (me->buttons() & Qt::LeftButton))
            {
                if(m_dragging)
                {
                    QPoint p = QCursor::pos();

                    qDebug("THT: Dropped at %d,%d", p.x(), p.y());
                    QApplication::restoreOverrideCursor();

                    emit dropped(m_startDragText, p);
                }

                m_dragging = false;
                m_startPos = QPoint();
            }
        }
    }

    return QObject::eventFilter(obj, event);
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
        items.append(item->text());

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

        if(Settings::instance()->tickerValidator().exactMatch(ticker))
        {
            changed = true;
            ui->list->addItem(ticker.toUpper().replace(QChar('-'), QChar('.')));
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

void List::showTickerInfo()
{
    QListWidgetItem *item = ui->list->currentItem();

    if(!item)
    {
        qDebug("THT: Nothing to load");
        return;
    }

    QPoint p = ui->list->viewport()->mapToGlobal(ui->list->visualItemRect(item).bottomLeft());

    if(p.isNull())
    {
        qDebug("THT: Cannot find where to show the information");
        return;
    }

    TickerInformationToolTip::showText(p, item->text());
}

QPixmap List::createDragCursor()
{
    QFont fnt = ui->list->font();
    int size = fnt.pointSize();

    if(size < 0)
        size = fnt.pixelSize();

    fnt.setPointSize(size+2);

    QFontMetrics fm(fnt);
    QSize dragCursorSize = fm.boundingRect(m_startDragText).adjusted(0,0, 10,4).size();

    QColor textColor = palette().color(QPalette::WindowText);
    QColor borderColor = QColor::fromRgb(0xffefef);

    QPixmap px(dragCursorSize);
    px.fill(Qt::transparent);
    QPainter p(&px);

    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // bounding rect, background
    QLinearGradient gr = QLinearGradient(QPointF(0, 0), QPointF(0, px.height()));
    gr.setColorAt(0, borderColor);
    gr.setColorAt(0.5, QColor::fromRgb(0xe6c207));
    gr.setColorAt(1, borderColor);

    p.setBrush(gr);
    p.setPen(Qt::NoPen);
    p.drawRect(px.rect());

    p.setPen(QPen(textColor, 1));
    p.setBrush(Qt::transparent);
    p.drawRect(px.rect());

    // text
    p.setFont(fnt);
    p.drawText(px.rect(), Qt::AlignCenter, m_startDragText);
    p.end();

    return px;
}

void List::loadItem(LoadItem litem)
{
    QListWidgetItem *item = ui->list->currentItem();

    if(!item)
        item = ui->list->item(0);

    switch(litem)
    {
        case LoadItemCurrent:
        break;

        case LoadItemNext:
            item = ui->list->item(ui->list->currentRow()+1);
        break;

        case LoadItemPrevious:
            item = ui->list->item(ui->list->currentRow()-1);
        break;

        case LoadItemFirst:
            item = ui->list->item(0);
        break;

        case LoadItemLast:
            item = ui->list->item(ui->list->count()-1);
        break;
    }

    if(!item)
    {
        qDebug("THT: Cannot find item to load");
        return;
    }

    ui->list->setCurrentItem(item);
    item->setSelected(true);
    emit loadTicker(item->text());
}

void List::slotAddOne()
{
    qDebug("THT: Adding one ticker");

    TickerInput ti(this);

    if(ti.exec() != QDialog::Accepted)
        return;

    QString ticker = ti.ticker();

    if(Settings::instance()->tickerValidator().exactMatch(ticker))
    {
        ui->list->addItem(ticker.toUpper().replace(QChar('-'), QChar('.')));
        numberOfItemsChanged();
        save();
    }
}

void List::slotAddFromFile()
{
    qDebug("THT: Adding new tickers from file");

    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose a file"), Settings::instance()->lastTickerDirectory());

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

    Settings::instance()->setLastTickerDirectory(QFileInfo(fileName).canonicalPath());

    bool changed = false;
    QTextStream t(&file);

    ui->list->setUpdatesEnabled(false);

    while(!t.atEnd())
    {
        t >> ticker;

        if(Settings::instance()->tickerValidator().exactMatch(ticker))
        {
            changed = true;
            ui->list->addItem(ticker.toUpper().replace('-', '.'));
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

    QString fileName = QFileDialog::getSaveFileName(this, tr("Choose a file"), Settings::instance()->lastTickerDirectory());

    if(fileName.isEmpty())
        return;

    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning("THT: Cannot open file for writing");
        QMessageBox::warning(this, tr("Error"), tr("Cannot save to file %1").arg(fileName));
        return;
    }

    Settings::instance()->setLastTickerDirectory(QFileInfo(fileName).canonicalPath());

    QTextStream t(&file);

    QStringList items = toStringList();

    foreach(QString item, items)
    {
        t << item << '\n';
    }

    t.flush();
}

void List::slotExportToClipboard()
{
    qDebug("THT: Exporting tickers to clipboard");

    QApplication::clipboard()->setText(toStringList().join("\n"));
}
