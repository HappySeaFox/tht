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

#include <QKeySequence>
#include <QKeyEvent>
#include <QEvent>

#include "tickerminiinput.h"
#include "uppercasevalidator.h"
#include "ui_tickerminiinput.h"

TickerMiniInput::TickerMiniInput(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TickerMiniInput)
{
    ui->setupUi(this);

    setFocusProxy(ui->line);

    ui->line->setValidator(new UpperCaseValidator(ui->line));
    ui->line->installEventFilter(this);
}

TickerMiniInput::~TickerMiniInput()
{
    delete ui;
}

bool TickerMiniInput::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == ui->line)
    {
        switch(e->type())
        {
            case QEvent::KeyPress:
            {
                QKeyEvent *ke = static_cast<QKeyEvent *>(e);

                // search widget
                switch(ke->key())
                {
                    case Qt::Key_Space:
                        emit addTicker(ui->line->text());
                        ui->line->clear();
                    break;

                    case Qt::Key_Return:
                    case Qt::Key_Enter:
                        emit loadTicker(ui->line->text());
                        ui->line->selectAll();
                    break;

                    case Qt::Key_Escape:
                        ui->line->clear();
                    break;
                }
            }
            break; // event type

            default:
            break;
        }
    }

    return QWidget::eventFilter(watched, e);
}
