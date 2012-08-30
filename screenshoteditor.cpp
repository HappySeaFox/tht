#include <QWhatsThisClickedEvent>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QApplication>
#include <QKeySequence>
#include <QScrollBar>
#include <QKeyEvent>
#include <QPixmap>
#include <QUrl>

#include "screenshoteditor.h"
#include "selectablelabel.h"
#include "ui_screenshoteditor.h"

ScreenshotEditor::ScreenshotEditor(const QPixmap &px, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreenshotEditor)
{
    ui->setupUi(this);
    ui->scrollAreaWidgetContents->setPixmap(px);
#warning TODO
    ui->scrollArea->setWhatsThis(QString("<a href=\"http://www.youtube.com/watch?v=\">%1</a>").arg(tr("Open YouTube tutorial")));

    connect(ui->pushText, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startText()));
    connect(ui->pushLong, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startBuy()));
    connect(ui->pushShort, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startSell()));
    connect(ui->pushStop, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startStop()));
    connect(ui->pushDelete, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(deleteSelected()));
    connect(ui->pushSelectAll, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(selectAll()));

    connect(ui->scrollAreaWidgetContents, SIGNAL(selected(SelectableLabel *, bool)), this, SLOT(slotSelected(SelectableLabel *, bool)));

    ui->pushDelete->setShortcut(QKeySequence::Delete);
    ui->pushSelectAll->setShortcut(QKeySequence::SelectAll);

    layout()->activate();

    const QRect rc = QDesktopWidget().availableGeometry(window());

    const int maxw = rc.width() * 0.75;
    const int maxh = rc.height() * 0.75;

    int neww = width() + (px.width() - ui->scrollArea->width() + 4);
    int newh = height() + (px.height() - ui->scrollArea->height() + 4);

    resize(qMax(qMin(neww, maxw), width()), qMax(qMin(newh, maxh), height()));
}

ScreenshotEditor::~ScreenshotEditor()
{
    delete ui;
}

QPixmap ScreenshotEditor::pixmap()
{
    return ui->scrollAreaWidgetContents->pixmap();
}

void ScreenshotEditor::restoreLabels()
{
    ui->scrollAreaWidgetContents->restoreLabels();
}

void ScreenshotEditor::saveLabels()
{
    ui->scrollAreaWidgetContents->saveLabels();
}

void ScreenshotEditor::clearLabels()
{
    ui->scrollAreaWidgetContents->clearLabels();
}

void ScreenshotEditor::keyPressEvent(QKeyEvent *ke)
{
    if(ke->key() == Qt::Key_Escape
            && ui->scrollAreaWidgetContents->editType() != ScreenshotEditorWidget::None)
    {
        ui->scrollAreaWidgetContents->cancel();
        return;
    }

    QDialog::keyPressEvent(ke);
}

bool ScreenshotEditor::event(QEvent *e)
{
    if(e->type() == QEvent::WhatsThisClicked)
    {
        QWhatsThisClickedEvent *ce = static_cast<QWhatsThisClickedEvent *>(e);

        if(ce)
            QDesktopServices::openUrl(QUrl(ce->href()));
    }

    return QDialog::event(e);
}

void ScreenshotEditor::slotSelected(SelectableLabel *sl, bool selected)
{
    if(!selected)
        return;

    const QList<SelectableLabel *> list = ui->scrollAreaWidgetContents->labels();

    if(QApplication::keyboardModifiers() != Qt::ControlModifier)
    {
        foreach(SelectableLabel *l, list)
        {
            if(l != sl)
                l->setSelected(false);
        }
    }
}
