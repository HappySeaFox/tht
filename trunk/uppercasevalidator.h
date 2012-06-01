#ifndef UPPERCASEVALIDATOR_H
#define UPPERCASEVALIDATOR_H

#include <QRegExpValidator>

class UpperCaseValidator : public QRegExpValidator
{
public:
    explicit UpperCaseValidator(QObject *parent = 0);

    QValidator::State validate(QString &input, int &pos) const;
};

#endif // UPPERCASEVALIDATOR_H
