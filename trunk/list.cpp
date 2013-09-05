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
#include <QStringList>
#include <QClipboard>
#include <QScrollBar>
#include <QDateTime>
#include <QFileInfo>
#include <QKeyEvent>
#include <QPalette>
#include <QPainter>
#include <QTimer>
#include <QLabel>
#include <QEvent>
#include <QColor>
#include <QMenu>
#include <QSize>
#include <QUrl>
#include <QPen>

#include "persistentselectiondelegate.h"
#include "tickerinformationtooltip.h"
#include "tickercommentinput.h"
#include "pluginimportexport.h"
#include "inlinetextinput.h"
#include "pluginloader.h"
#include "tickerinput.h"
#include "listdetails.h"
#include "settings.h"
#include "listitem.h"
#include "list.h"

#include "ui_list.h"

List::List(int group, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::List),
    m_section(group),
    m_saveTickers(SETTINGS_GET_BOOL(SETTING_SAVE_TICKERS)),
    m_ignoreInput(false),
    m_dragging(false),
    m_currentItemBeforeSearch(0),
    m_scrollPos(-1)
{
    ui->setupUi(this);

    // header widgets
    ui->widgetEnterHeader->setMaximumLength(16);
    ui->labelHeader->setText(Settings::instance()->headerForGroup(m_section));
    ui->widgetSearch->setUseTickerValidator(true);

    reconfigureMiniTickerEntry();

    connect(ui->widgetInput, SIGNAL(focusUp()), this, SLOT(slotFocusUp()));
    connect(ui->widgetInput, SIGNAL(addTicker(QString)), this, SLOT(slotAddTicker(QString)));
    connect(ui->widgetInput, SIGNAL(loadTicker(QString)), this, SIGNAL(loadTicker(QString)));

    // focus proxies
    ui->stack->widget(0)->setFocusProxy(ui->widgetInput);
    ui->stack->widget(1)->setFocusProxy(ui->widgetSearch);

    ui->stackHeader->widget(0)->setFocusProxy(ui->labelHeader);
    ui->stackHeader->widget(1)->setFocusProxy(ui->widgetEnterHeader);

    m_persistentDelegate = new PersistentSelectionDelegate(ui->list);
    m_oldDelegate = ui->list->itemDelegate();

    // number of tickers
    m_numbers = new ListDetails(window());

    ui->pushSave->setEnabled(!m_saveTickers);

    QMenu *menu = new QMenu(this);
    //: Infinitive
    menu->addAction(QIcon(":/images/clear.png"), tr("Clear") + '\t' + QKeySequence(QKeySequence::New).toString(), this, SLOT(clear()));
    menu->addSeparator();
    //: Infinitive
    menu->addAction(tr("Sort") + "\tR", this, SLOT(slotSortList()));
    m_changeTitle = menu->addAction(tr("Change title") + "\tF2", this, SLOT(changeHeader()));
    menu->addSeparator();
    menu->addAction(tr("Reset priorities") + "\tAlt+U", this, SLOT(slotResetPriorities()));

    ui->pushList->setMenu(menu);

    QIcon file_icon(":/images/file.png");

    menu = new QMenu(this);
    menu->addAction(tr("Add one ticker...") + "\tO", this, SLOT(slotAddOne()));
    menu->addSeparator();
    menu->addAction(file_icon, tr("Add from file...") + "\tA", this, SLOT(slotAddFromFile()));
    menu->addAction(tr("Add from clipboard") + "\tP", this, SLOT(paste()));
    ui->pushAdd->setMenu(menu);

    embedPlugins(Plugin::AddTickersFrom, menu);

    menu = new QMenu(this);
    menu->addAction(file_icon, tr("Export to file...") + "\tE", this, SLOT(slotExportToFile()));
    menu->addAction(tr("Export to clipboard") + "\tC", this, SLOT(slotExportToClipboard()));
    ui->pushSaveAs->setMenu(menu);

    embedPlugins(Plugin::ExportTickersTo, menu);

    setFocusProxy(ui->list);

    load();
    numberOfItemsChanged();

    // catch keyboard events
    ui->list->installEventFilter(this);
    ui->list->viewport()->installEventFilter(this);
    ui->labelHeader->installEventFilter(this);

    m_numbers->show();
}

List::~List()
{
    delete m_numbers;
    delete ui;
}

bool List::hasTickers() const
{
    return ui->list->count();
}

void List::addTicker(const Ticker &ticker)
{
    if(addItem(ticker.ticker
               + ','
               + QString::number(ticker.priority)
               + ','
               + ticker.comment.toUtf8().toPercentEncoding(),
               DontFix, CheckDups))
    {
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

void List::showHeader(bool sh)
{
    ui->widgetHeader->setVisible(sh);
    m_changeTitle->setEnabled(sh);
}

void List::showButtons(bool sh)
{
    ui->widgetButtons->setVisible(sh);
}

bool List::contains(const QPoint &p)
{
    return ui->list->geometry().contains(mapFromGlobal(p));
}

void List::initialSelect(bool alsoSetFocus)
{
    if(alsoSetFocus)
        setFocus();

    QListWidgetItem *item = ui->list->item(0);

    if(!item)
        return;

    ui->list->setCurrentItem(item);
}

void List::removeDuplicates()
{
    qDebug("Looking for duplicates in list #%d", m_section);

    int row = 0;
    QListWidgetItem *i;
    QList<QListWidgetItem *> list;
    bool changed = false;

    while((i = ui->list->item(row++)))
    {
        list = ui->list->findItems(i->text(), Qt::MatchFixedString);

        if(list.size() > 1)
        {
            changed = true;

            for(int i = 1;i < list.size();i++)
                delete list[i];
        }
    }

    if(changed)
    {
        numberOfItemsChanged();
        save();
    }
}

void List::stopSearching()
{
    qDebug("Searching ticker has finished");

    // set old selected item
    if(!ui->list->currentItem())
        ui->list->setCurrentItem(m_currentItemBeforeSearch);

    // revert old delegate
    ui->list->setItemDelegate(m_oldDelegate);

    m_foundItems.clear();
    ui->widgetSearch->stopEditing();

    if(window()->focusWidget()->objectName() != "list")
        setFocus();

    if(SETTINGS_GET_BOOL(SETTING_MINI_TICKER_ENTRY))
        ui->stack->setCurrentIndex(0);
    else
        ui->stack->hide();
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

            bool ate = true;

            if(ke->matches(QKeySequence::Paste))
                paste();
            else if(ke->matches(QKeySequence::New))
                clear();
            else if(ke->matches(QKeySequence::Open))
                slotAddFromFile();
            else if(ke->matches(QKeySequence::Save))
                slotSave();
            else if(ke->matches(QKeySequence::Undo))
                undo();
            else if(ke->modifiers() == Qt::NoModifier
                    || ke->modifiers() == Qt::KeypadModifier) // disallow all modifiers except keypad
            {
                switch(ke->key())
                {
                    case Qt::Key_F2:
                        changeHeader();
                    break;

                    case Qt::Key_1:
                    case Qt::Key_2:
                    case Qt::Key_3:
                    case Qt::Key_4:
                    case Qt::Key_5:
                    case Qt::Key_6:
                    case Qt::Key_7:
                    case Qt::Key_8:
                        emit copyTo(currentTickerInfo(), ke->key() - Qt::Key_1);
                    break;

                    case Qt::Key_A:
                        slotAddFromFile();
                    break;

                    case Qt::Key_P:
                        paste();
                    break;

                    case Qt::Key_C:
                        slotExportToClipboard();
                    break;

                    case Qt::Key_E:
                        slotExportToFile();
                    break;

                    case Qt::Key_O:
                    case Qt::Key_Insert:
                        slotAddOne();
                    break;

                    case Qt::Key_Right:
                        emit copyRight(currentTickerInfo());
                    break;

                    case Qt::Key_Left:
                        emit copyLeft(currentTickerInfo());
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
                    case Qt::Key_Enter:
                        loadItem(LoadItemCurrent);
                    break;

                    case Qt::Key_R:
                        slotSortList();
                    break;

                    // Finviz
                    case Qt::Key_F:
                        openTickerInBrowser("http://finviz.com/quote.ashx?t=%1", currentTicker(), Fix);
                    break;

                    // Google finance
                    case Qt::Key_G:
                        openTickerInBrowser("http://www.google.com/finance?q=%1", currentTicker(), DontFix);
                    break;

                    // Yahoo finance
                    case Qt::Key_Y:
                        openTickerInBrowser("http://finance.yahoo.com/q?s=%1", currentTicker(), Fix);
                    break;

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
                        loadItem(LoadItemPageUp);
                    break;

                    case Qt::Key_PageDown:
                        loadItem(LoadItemPageDown);
                    break;

                    case Qt::Key_Plus:
                    case Qt::Key_Equal:
                        changePriority(+1);
                    break;

                    case Qt::Key_Minus:
                        changePriority(-1);
                    break;

                    case Qt::Key_U:
                        slotResetPriority();
                    break;

                    case Qt::Key_K:
                        emit showNeighbors(currentTicker());
                    break;

                    case Qt::Key_X:
                        showComment();
                    break;

                    // default processing
                    case Qt::Key_Tab:
                        return QObject::eventFilter(obj, event);

                    default:
                        ate = false;
                    break;
                } // switch
            }
            else if(ke->modifiers() == Qt::ControlModifier)
            {
                switch(ke->key())
                {
                    case Qt::Key_Up:
                        moveItem(MoveItemPreviuos);
                    break;

                    case Qt::Key_Down:
                        moveItem(MoveItemNext);
                    break;

                    case Qt::Key_Home:
                        moveItem(MoveItemFirst);
                    break;

                    case Qt::Key_End:
                        moveItem(MoveItemLast);
                    break;

                    case Qt::Key_PageUp:
                        moveItem(MoveItemPageUp);
                    break;

                    case Qt::Key_PageDown:
                        moveItem(MoveItemPageDown);
                    break;

                    default:
                        ate = false;
                    break;
                }
            }
            else if(ke->modifiers() == Qt::AltModifier)
            {
                switch(ke->key())
                {
                    case Qt::Key_U:
                        slotResetPriorities();
                    break;

                    case Qt::Key_1:
                    case Qt::Key_2:
                    case Qt::Key_3:
                    case Qt::Key_4:
                        setPriority(ke->key() - Qt::Key_1);
                    break;

                    case Qt::Key_X:
                        changeComment();
                    break;

                    default:
                        ate = false;
                    break;
                }
            }
            else
                ate = false;

            if(!ate)
            {
                qDebug("Sending keyboard input to plugins");

                foreach(PluginImportExport *p, m_plugins)
                {
                    QList<Hotkey> hotkeys = p->supportedHotkeysInList();

                    foreach(Hotkey hotkey, hotkeys)
                    {
                        if(ke->key() == hotkey.key && ke->modifiers() == hotkey.modifiers)
                        {
                            p->listHotkeyActivated(m_section, hotkey);
                            break;
                        }
                    }
                }
            }

            return true;
        }
        else if(type == QEvent::FocusIn)
            ui->list->setAlternatingRowColors(true);
        else if(type == QEvent::FocusOut)
            ui->list->setAlternatingRowColors(false);
        else if(type == QEvent::Resize || type == QEvent::Move)
            moveNumbersLabel();
    }
    else if(obj == ui->list->viewport())
    {
        if(type == QEvent::MouseButtonPress)
        {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);

            if(me->buttons() & Qt::LeftButton)
            {
                ListItem *i = static_cast<ListItem *>(ui->list->itemAt(me->pos()));
                m_startDragTicker.ticker = i ? i->text() : QString();

                if(!m_startDragTicker.ticker.isEmpty())
                {
                    m_startDragTicker.priority = i->priority();
                    m_startDragTicker.comment = i->comment();
                    m_startPos = me->pos();
                }
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
                    qDebug("Start dragging \"%s\"", qPrintable(m_startDragTicker.ticker));

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
                    emit tickerCancelled();
                }
                else
                    emit tickerMoving(me->globalPos());
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

                    qDebug("Dropped at %d,%d", p.x(), p.y());
                    QApplication::restoreOverrideCursor();

                    emit tickerDropped(m_startDragTicker, p);
                }

                m_dragging = false;
                m_startPos = QPoint();
            }
        }
        else if(type == QEvent::Resize || type == QEvent::Move)
            moveNumbersLabel();
    }
    else if(obj == ui->labelHeader)
    {
        if(type == QEvent::MouseButtonDblClick)
            changeHeader();
    }

    return QObject::eventFilter(obj, event);
}

void List::moveEvent(QMoveEvent *)
{
    moveNumbersLabel();
}

void List::numberOfItemsChanged()
{
    m_numbers->setTotal(ui->list->count());

    // also change the current row
    slotCurrentRowChanged(ui->list->currentRow());

    resizeNumberLabel();

    if(ui->list->count())
        m_oldTickers.clear();
}

QStringList List::toStringList(int flags)
{
    int i = 0;
    QStringList items;
    ListItem *item;
    bool withExtra = (flags & WithExtraData);

    while((item = static_cast<ListItem *>(ui->list->item(i++))))
    {
        items.append(withExtra
                     ? (item->text()
                        + ','
                        + QString::number(item->priority())
                        + ','
                        + item->comment().toUtf8().toPercentEncoding())
                     : item->text());
    }

    return items;
}

void List::save()
{
    // don't save tickers automatically
    if(!m_saveTickers)
    {
        qDebug("Autosave is disabled");
        showSaved(false);
        return;
    }

    slotSave();
}

void List::load()
{
    qDebug("Loading tickers from section \"%d\"", m_section);
    addTickers(Settings::instance()->tickersForGroup(m_section), DontFix);
}

void List::paste()
{
    addTickers(QApplication::clipboard()->text().split(QRegExp("\\s+"), QString::SkipEmptyParts), Fix);
}

void List::headerCancelled()
{
    ui->stackHeader->setCurrentIndex(0);

    if(window()->focusWidget()->objectName() != "list")
        setFocus();
}

void List::headerAccepted()
{
    ui->labelHeader->setText(ui->widgetEnterHeader->text());
    Settings::instance()->setHeaderForGroup(m_section, ui->labelHeader->text());

    headerCancelled();
}

void List::changeHeader()
{
    if(!SETTINGS_GET_BOOL(SETTING_LIST_HEADER))
        return;

    ui->widgetEnterHeader->startEditing(ui->labelHeader->text(), true);
    ui->stackHeader->setCurrentIndex(1);
    ui->stackHeader->currentWidget()->setFocus();
}

void List::slotBeforeSqueeze()
{
    // save scroll position
    m_scrollPos = ui->list->verticalScrollBar()->value();
}

void List::slotSqueezed(bool s)
{
    // restore scroll position
    if(!s)
        ui->list->verticalScrollBar()->setValue(m_scrollPos);
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
        qDebug("Nothing to load");
        return;
    }

    QRect rc = ui->list->visualItemRect(item);

    if(!rc.isValid())
    {
        qDebug("Cannot find where to show the information");
        return;
    }

    TickerInformationToolTip::showText(ui->list->viewport()->mapToGlobal(rc.bottomLeft()), item->text());
}

QPixmap List::createDragCursor()
{
    QFont fnt = ui->list->font();
    int size = fnt.pointSize();

    if(size < 0)
        size = fnt.pixelSize();

    fnt.setPointSize(size+2);

    QFontMetrics fm(fnt);
    QSize dragCursorSize = fm.boundingRect(m_startDragTicker.ticker).adjusted(0,0, 10,4).size();

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
    p.drawText(px.rect(), Qt::AlignCenter, m_startDragTicker.ticker);
    p.end();

    return px;
}

void List::addTickers(const QStringList &tk, FixName fix)
{
    QStringList tickers = tk;

    // nothing to paste
    if(tickers.isEmpty())
    {
        qDebug("Nothing to add");
        return;
    }

    qint64 v = QDateTime::currentMSecsSinceEpoch();

    CheckForDups check;

    if(!ui->list->count())
    {
        check = DontCheckDups;

        if(!SETTINGS_GET_BOOL(SETTING_ALLOW_DUPLICATES))
        {
            qDebug("Fast remove dups");
            tickers.removeDuplicates();
        }
    }
    else
        check = SETTINGS_GET_BOOL(SETTING_ALLOW_DUPLICATES) ? DontCheckDups : CheckDups;

    ui->list->setUpdatesEnabled(false);

    bool changed = false;

    foreach(QString ticker, tickers)
    {
        if(addItem(ticker, fix, check))
            changed = true;
    }

    ui->list->setUpdatesEnabled(true);

    qDebug("Added in %ld ms.", static_cast<long int>(QDateTime::currentMSecsSinceEpoch() - v));

    if(changed)
    {
        numberOfItemsChanged();
        save();
    }
}

bool List::addItem(const QString &txt, FixName fix, CheckForDups check)
{
    QStringList text = txt.split(',');

    if(text.isEmpty())
        return false;

    text[0] = text[0].toUpper();

    if(fix == Fix)
        text[0].replace('-', '.');

    if(!Settings::instance()->tickerValidator().exactMatch(text[0]))
        return false;

    if(check == CheckDups
            && !SETTINGS_GET_BOOL(SETTING_ALLOW_DUPLICATES)
            && ui->list->findItems(text.at(0), Qt::MatchFixedString).size())
        return false;

    // check if empty
    bool wasEmpty = !ui->list->count();

    ListItem *item = new ListItem(text.at(0), ui->list);

    if(text.size() > 1)
    {
        bool ok;
        int p = text.at(1).toInt(&ok);

        if(ok)
            item->setPriority(static_cast<Ticker::Priority>(p));
    }

    if(text.size() > 2)
    {
        QString comment = QString::fromUtf8(QByteArray::fromPercentEncoding(text[2].toLatin1()));

        if(!comment.isEmpty())
        {
            item->setComment(comment);

            QFont f = item->font();
            f.setBold(true);
            item->setFont(f);
        }
    }

    ui->list->addItem(item);

    if(wasEmpty)
        initialSelect(ui->list->hasFocus());

    return true;
}

void List::changePriority(int p)
{
    if(!p)
    {
        qDebug("Won't set the same priority");
        return;
    }

    ListItem *li = static_cast<ListItem *>(ui->list->currentItem());

    if(!li)
    {
        qDebug("Cannot find ticker");
        return;
    }

    // fix the value
    if(p < -1)
        p = -1;
    else if(p > +1)
        p = +1;

    if((p < 0 && li->priority() == Ticker::PriorityNormal)
            || (p > 0 && li->priority() == Ticker::PriorityHighest))
    {
        qDebug("Priority is on the edge");
        return;
    }

    qDebug("Changing priority %+d", p);

    li->setPriority(static_cast<Ticker::Priority>(li->priority() + p));

    save();
}

void List::setPriority(int p)
{
    ListItem *li = static_cast<ListItem *>(ui->list->currentItem());

    if(!li)
    {
        qDebug("Cannot find ticker");
        return;
    }

    Ticker::Priority pr = static_cast<Ticker::Priority>(p);

    if(pr == li->priority())
    {
        qDebug("Won't set the same priority");
        return;
    }

    qDebug("Setting priority %d", p);

    li->setPriority(pr);

    save();
}

void List::resizeNumberLabel()
{
    m_numbers->ensurePolished();

    // basic size
    QSize size = QFontMetrics(m_numbers->font()).size(0, m_numbers->totalText());
    size.setHeight(size.height()*2);

    // plus margins
    int left = 0, top = 0, right = 0, bottom = 0;

    m_numbers->layout()->getContentsMargins(&left, &top, &right, &bottom);
    m_numbers->resize(size + QSize(left + right + 2, top + bottom + m_numbers->layout()->spacing() + 2));

    moveNumbersLabel();
}

void List::moveNumbersLabel()
{
    const QWidget *w = ui->list->viewport();

    m_numbers->move(w->mapTo(window(), QPoint(w->width(), 0)).x() - m_numbers->width(),
                   w->mapTo(window(), QPoint(0, w->height())).y() - m_numbers->height()/2 + 1);
}

void List::undo()
{
    if(ui->list->count() || m_oldTickers.isEmpty())
    {
        qDebug("Nothing to undo");
        return;
    }

    qDebug("Undo");

    addTickers(m_oldTickers, DontFix);
}

void List::openTickerInBrowser(const QString &baseUrl, const QString &ticker, List::FixName fix)
{
    if(baseUrl.isEmpty() || ticker.isEmpty())
        return;

    QDesktopServices::openUrl(QUrl::fromUserInput(baseUrl.arg(fix == Fix ? QString(ticker).replace('.', '-') : ticker)));
}

void List::changeComment()
{
    ListItem *item = static_cast<ListItem *>(ui->list->currentItem());

    if(!item)
    {
        qDebug("Won't change comment on this item");
        return;
    }

    TickerCommentInput tci(item->comment(), this);

    if(tci.exec() == QDialog::Accepted)
    {
        item->setComment(tci.comment());

        QFont f = item->font();
        f.setBold(!item->comment().isEmpty());
        item->setFont(f);

        save();
    }
}

void List::showComment()
{
    ListItem *item = static_cast<ListItem *>(ui->list->currentItem());

    if(item)
    {
        QRect rc = ui->list->visualItemRect(item);

        if(rc.isValid())
            TickerInformationToolTip::showPersistentText(ui->list->viewport()->mapToGlobal(rc.bottomLeft()), item->comment());
        else
            qDebug("Cannot find where to show the comment");
    }
}

Ticker List::currentTickerInfo() const
{
    Ticker t;
    ListItem *item = static_cast<ListItem *>(ui->list->currentItem());

    if(!item)
    {
        qDebug("Cannot get the current ticker");
        return t;
    }

    t.ticker = item->text();
    t.priority = item->priority();
    t.comment = item->comment();

    return t;
}

void List::embedPlugins(Plugin::Type type, QMenu *menu)
{
    if(!menu)
        return;

    QList<Plugin *> plugins = PluginLoader::instance()->byType(type);

    if(!plugins.isEmpty())
        menu->addSeparator();

    foreach(Plugin *p, plugins)
    {
        PluginImportExport *pie = qobject_cast<PluginImportExport *>(p);

        if(!pie)
            continue;

        pie->embed(m_section, menu);
        connect(pie, SIGNAL(sendTickers(int,QStringList)), this, SLOT(slotSentTickersFromPlugin(int,QStringList)));
        connect(pie, SIGNAL(requestTickers(int)), this, SLOT(slotRequestedTickersFromPlugin(int)));

        m_plugins.append(pie);
    }
}

void List::loadItem(LoadItem litem)
{
    QListWidgetItem *item = ui->list->currentItem();

    if(!item)
        item = ui->list->item(0);

    QListWidgetItem *citem = item;

    switch(litem)
    {
        case LoadItemCurrent:
        break;

        case LoadItemNext:
            item = ui->list->item(ui->list->row(item)+1);
        break;

        case LoadItemPrevious:
            item = ui->list->item(ui->list->row(item)-1);
        break;

        case LoadItemFirst:
            item = ui->list->item(0);
        break;

        case LoadItemLast:
            item = ui->list->item(ui->list->count()-1);
        break;

        case LoadItemPageUp:
        case LoadItemPageDown:
            item = ui->list->itemAt(ui->list->visualItemRect(item).adjusted(
                                        0,
                                        (litem == LoadItemPageUp ? -ui->list->viewport()->height() : ui->list->viewport()->height()),
                                        0,
                                        0
                                        ).topLeft());

            if(!item)
                item = (litem == LoadItemPageUp) ? ui->list->item(0) : ui->list->item(ui->list->count()-1);
        break;
    }

    if(!item)
    {
        qDebug("Cannot find item to load");
        return;
    }

    if(item == citem && litem != LoadItemCurrent)
    {
        qDebug("Will not load the same item");
        return;
    }

    ui->list->setCurrentItem(item);
    emit loadTicker(item->text());

    if(SETTINGS_GET_BOOL(SETTING_SHOW_COMMENTS))
        showComment();
}

void List::moveItem(MoveItem mi)
{
    QListWidgetItem *item = ui->list->currentItem();

    if(!item)
    {
        qDebug("Cannot find the ticker to move");
        return;
    }

    int crow = ui->list->currentRow();
    int row = -1;

    switch(mi)
    {
        case MoveItemNext:
            row = ui->list->row(item)+1;

            if(row >= ui->list->count())
                row = ui->list->count()-1;
        break;

        case MoveItemPreviuos:
            row = ui->list->row(item)-1;

            if(row <= 0)
                row = 0;
        break;

        case MoveItemFirst:
            row = 0;
        break;

        case MoveItemLast:
            row = ui->list->count()-1;
        break;

        case MoveItemPageUp:
        case MoveItemPageDown:
            item = ui->list->itemAt(ui->list->visualItemRect(item).adjusted(
                                        0,
                                        (mi == MoveItemPageUp ? -ui->list->viewport()->height() : ui->list->viewport()->height()),
                                        0,
                                        0
                                        ).topLeft());

            if(!item)
                item = (mi == MoveItemPageUp) ? ui->list->item(0) : ui->list->item(ui->list->count()-1);

            row = ui->list->row(item);
        break;
    }

    if(row == crow)
    {
        qDebug("Won't move to the same position");
        return;
    }

    if(row < 0)
    {
        qDebug("Won't move to the invalid position");
        return;
    }

    qDebug("Moving ticker from position %d to %d", crow, row);

    item = ui->list->takeItem(crow);

    if(!item)
        return;

    ui->list->insertItem(row, item);
    ui->list->setCurrentItem(item);

    save();
}

void List::focusMiniTickerEntry()
{
    if(SETTINGS_GET_BOOL(SETTING_MINI_TICKER_ENTRY))
    {
        ui->stack->setCurrentIndex(0);
        ui->widgetInput->setFocusAndSelect();
        ui->widgetInput->flash();
    }
}

void List::slotAddOne()
{
    qDebug("Adding one ticker");

    if(SETTINGS_GET_BOOL(SETTING_MINI_TICKER_ENTRY))
        focusMiniTickerEntry();
    else
    {
        TickerInput ti(this);

        if(ti.exec() != QDialog::Accepted
                || !addItem(ti.ticker().toUpper(), Fix, CheckDups))
            return;

        numberOfItemsChanged();
        save();
    }
}

void List::slotAddTicker(const QString &t)
{
    addTicker(Ticker(t));
}

void List::slotAddFromFile()
{
    qDebug("Adding new tickers from file");

    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Choose a file"),
                                                          SETTINGS_GET_STRING(SETTING_LAST_TICKER_DIRECTORY),
                                                          tr("Text files (*.txt)")
                                                              + ";;"
                                                              + tr("All files (*.*)"));

    if(fileNames.isEmpty())
        return;

    bool error = false;
    QStringList errorFiles, tickers;

    SETTINGS_SET_STRING(SETTING_LAST_TICKER_DIRECTORY, QFileInfo(fileNames[0]).absolutePath());

    foreach(QString fileName, fileNames)
    {
        QString ticker;
        QFile file(fileName);

        if(!file.open(QIODevice::ReadOnly))
        {
            qWarning("Cannot open file \"%s\" for reading", qPrintable(fileName));
            error = true;
            errorFiles.append(fileName);
            continue;
        }

        QTextStream t(&file);

        while(!t.atEnd())
        {
            t >> ticker;
            tickers.append(ticker);
        }
    }

    addTickers(tickers, Fix);

    if(error)
        QMessageBox::warning(this, tr("Error"), tr("Cannot open the following files: %1").arg(errorFiles.join(",")));
}

void List::clear()
{
    qDebug("Clear tickers for section \"%d\"", m_section);

    // nothing to do
    if(!ui->list->count())
        return;

    m_oldTickers = toStringList(WithExtraData);

    ui->list->clear();
    numberOfItemsChanged();
    save();
}

void List::slotSave()
{
    qDebug("Saving tickers to section \"%d\"", m_section);

    qint64 t = QDateTime::currentMSecsSinceEpoch();

    Settings::instance()->setTickersForGroup(m_section, toStringList(WithExtraData));

    showSaved(true);

    qDebug("Saved in %ld ms.", static_cast<long int>(QDateTime::currentMSecsSinceEpoch() - t));
}

void List::slotExportToFile()
{
    qDebug("Exporting tickers to file");

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Choose a file"),
                                                    SETTINGS_GET_STRING(SETTING_LAST_TICKER_DIRECTORY),
                                                    tr("Text files (*.txt)")
                                                        + ";;"
                                                        + tr("All files (*.*)"));

    if(fileName.isEmpty())
        return;

    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        qWarning("Cannot open file for writing");
        QMessageBox::warning(this, tr("Error"), tr("Cannot save to file %1").arg(fileName));
        return;
    }

    SETTINGS_SET_STRING(SETTING_LAST_TICKER_DIRECTORY, QFileInfo(fileName).absolutePath());

    QTextStream t(&file);

    QStringList items = toStringList();

    foreach(QString item, items)
    {
        t << item << '\n';
    }

    t.flush();
}

void List::slotSortList()
{
    ui->list->sortItems();
    save();
}

void List::slotResetPriorities()
{
    int row = 0;
    ListItem *i;

    while((i = static_cast<ListItem *>(ui->list->item(row++))))
        i->setPriority(Ticker::PriorityNormal);

    save();
}

void List::slotResetPriority()
{
    ListItem *i = static_cast<ListItem *>(ui->list->currentItem());

    if(!i || i->priority() == Ticker::PriorityNormal)
        return;

    qDebug("Resetting priority for ticker \"%s\"", qPrintable(i->text()));

    i->setPriority(Ticker::PriorityNormal);
    save();
}

void List::startSearching()
{
    qDebug("Start searching a ticker");

    m_foundItems.clear();

    m_currentItemBeforeSearch = ui->list->currentItem();

    ui->list->setItemDelegate(m_persistentDelegate);

    ui->widgetSearch->startEditing();
    ui->stack->setCurrentIndex(1);
    ui->stack->currentWidget()->setFocus();

    if(!SETTINGS_GET_BOOL(SETTING_MINI_TICKER_ENTRY))
        ui->stack->show();
}

bool List::searching() const
{
    return SETTINGS_GET_BOOL(SETTING_MINI_TICKER_ENTRY) ? ui->stack->currentIndex() : ui->stack->isVisible();
}

void List::reconfigureMiniTickerEntry()
{
    if(SETTINGS_GET_BOOL(SETTING_MINI_TICKER_ENTRY))
    {
        ui->stack->show();
        ui->stack->setCurrentIndex(0);
    }
    else
        ui->stack->hide();
}

void List::slotSearchTicker(const QString &ticker)
{
    if(ticker.isEmpty())
    {
        ui->list->setCurrentItem(0);
        return;
    }

    m_foundItems = ui->list->findItems(ticker, Qt::MatchStartsWith);

    ui->list->setCurrentItem(m_foundItems.isEmpty() ? 0 : m_foundItems[0]);
}

void List::slotSearchTickerNext()
{
    // nothing to cycle
    if(m_foundItems.size() < 2)
        return;

    int index = m_foundItems.indexOf(ui->list->currentItem());

    if(index < 0)
        return;

    index++;

    if(index >= m_foundItems.size())
        index = 0;

    if(m_foundItems[index])
        ui->list->setCurrentItem(m_foundItems[index]);
}

void List::slotFocusUp()
{
    setFocus();
}

void List::slotCurrentRowChanged(int row)
{
    m_numbers->setCurrent(row+1);
}

void List::slotSentTickersFromPlugin(int list, const QStringList &tickers)
{
    if(list == m_section)
        addTickers(tickers, Fix);
}

void List::slotRequestedTickersFromPlugin(int list)
{
    if(list == m_section)
    {
        PluginImportExport *pie = qobject_cast<PluginImportExport *>(sender());

        if(pie)
            pie->exportTickers(toStringList(WithExtraData));
    }
}

void List::slotExportToClipboard()
{
    qDebug("Exporting tickers to clipboard");

    QApplication::clipboard()->setText(toStringList().join("\n"));
}
