#ifndef TICKERINPUT_H
#define TICKERINPUT_H

#include <QDialog>

namespace Ui {
class TickerInput;
}

class TickerInput : public QDialog
{
    Q_OBJECT
    
public:
    explicit TickerInput(QWidget *parent = 0);
    ~TickerInput();

    QString ticker() const;

private:
    Ui::TickerInput *ui;
};

#endif // TICKERINPUT_H
