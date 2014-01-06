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

#include <QKeyEvent>

#include "tickerinformationtooltip.h"
#include "uppercasevalidator.h"
#include "thtsettings.h"
#include "tickerinput.h"
#include "settings.h"

#include "ui_tickerinput.h"

TickerInput::TickerInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TickerInput)
{
    ui->setupUi(this);
    ui->line->setValidator(new UpperCaseValidator(ui->line));
    ui->line->installEventFilter(this);

    ui->line->setText(SETTINGS_GET_STRING(SETTING_LAST_TICKER_INPUT));
    ui->line->selectAll();
}

TickerInput::~TickerInput()
{
    delete ui;
}

QString TickerInput::ticker() const
{
    return ui->line->text();
}

void TickerInput::slotAccepted()
{
    SETTINGS_SET_STRING(SETTING_LAST_TICKER_INPUT, ui->line->text());
}

bool TickerInput::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);

        if(ke && ke->key() == Qt::Key_Space)
        {
            TickerInformationToolTip::showText(ui->line->mapToGlobal(QPoint(0, ui->line->height())),
                                               ui->line->text());

            return true;
        }
    }

    return QObject::eventFilter(o, e);
}
