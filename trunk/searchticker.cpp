#include <QRegExpValidator>
#include <QKeyEvent>
#include <QTimer>
#include <QEvent>

#include "searchticker.h"
#include "uppercasevalidator.h"
#include "ui_searchticker.h"

SearchTicker::SearchTicker(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint),
    ui(new Ui::SearchTicker)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    setFocusProxy(ui->line);

    ui->line->setValidator(new UpperCaseValidator(ui->line));

    connect(ui->pushClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->line, SIGNAL(textChanged(const QString &)), this, SIGNAL(ticker(const QString &)));

    ui->line->installEventFilter(this);
}

SearchTicker::~SearchTicker()
{
    delete ui;
}

bool SearchTicker::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == ui->line)
    {
        bool doit = false;

        switch(e->type())
        {
            case QEvent::FocusOut:
                doit = true;
            break;

            case QEvent::KeyPress:
            {
                QKeyEvent *ke = static_cast<QKeyEvent *>(e);

                if(ke->key() == Qt::Key_Escape
                        || ke->key() == Qt::Key_Return
                        || ke->key() == Qt::Key_Enter)
                    doit = true;
            }
            break;

            default:
            break;
        }

        if(doit)
        {
            QTimer::singleShot(0, this, SLOT(close()));
            return true;
        }
    }

    return QWidget::eventFilter(watched, e);
}
