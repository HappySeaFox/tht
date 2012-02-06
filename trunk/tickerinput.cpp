#include <QRegExpValidator>

#include "settings.h"
#include "tickerinput.h"
#include "ui_tickerinput.h"

TickerInput::TickerInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TickerInput)
{
    ui->setupUi(this);
    ui->line->setValidator(new QRegExpValidator(Settings::instance()->tickerValidator()));
}

TickerInput::~TickerInput()
{
    delete ui;
}

QString TickerInput::ticker() const
{
    return ui->line->text();
}
