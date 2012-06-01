#include "settings.h"
#include "uppercasevalidator.h"

UpperCaseValidator::UpperCaseValidator(QObject *parent)
    : QRegExpValidator(Settings::instance()->tickerValidator(), parent)
{}

QValidator::State UpperCaseValidator::validate(QString &input, int &pos) const
{
    input = input.toUpper();
    return QRegExpValidator::validate(input, pos);
}
