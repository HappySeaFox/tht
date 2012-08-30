#include <QApplication>
#include <QKeySequence>
#include <QKeyEvent>
#include <QPixmap>

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
    connect(ui->pushLong, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startLong()));
    connect(ui->pushShort, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startShort()));
    connect(ui->pushStop, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(startStop()));
    connect(ui->pushDelete, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(deleteSelected()));
    connect(ui->pushSelectAll, SIGNAL(clicked()), ui->scrollAreaWidgetContents, SLOT(selectAll()));

    connect(ui->scrollAreaWidgetContents, SIGNAL(selected(SelectableLabel *, bool)), this, SLOT(slotSelected(SelectableLabel *, bool)));

    ui->pushDelete->setShortcut(QKeySequence::Delete);
    ui->pushSelectAll->setShortcut(QKeySequence::SelectAll);
}

ScreenshotEditor::~ScreenshotEditor()
{
    delete ui;
}

QPixmap ScreenshotEditor::pixmap()
{
    return ui->scrollAreaWidgetContents->pixmap();
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

void ScreenshotEditor::slotSelected(SelectableLabel *sl, bool selected)
{
    if(!selected)
        return;

    QList<SelectableLabel *> list = ui->scrollAreaWidgetContents->labels();

    if(QApplication::keyboardModifiers() != Qt::ControlModifier)
    {
        foreach(SelectableLabel *l, list)
        {
            if(l != sl)
                l->setSelected(false);
        }
    }
}
