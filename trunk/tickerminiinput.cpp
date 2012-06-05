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
#include <QTimer>
#include <QBrush>
#include <QEvent>

#include "settings.h"
#include "tickerminiinput.h"
#include "uppercasevalidator.h"
#include "ui_tickerminiinput.h"

TickerMiniInput::TickerMiniInput(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TickerMiniInput)
{
    ui->setupUi(this);

    m_timer = new QTimer(this);
    m_timer->setInterval(250);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotRevertPalette()));

    setFocusProxy(ui->line);

    ui->line->setValidator(new UpperCaseValidator(ui->line));
    ui->line->installEventFilter(this);
}

TickerMiniInput::~TickerMiniInput()
{
    delete ui;
}

void TickerMiniInput::flash()
{
    QPalette p = ui->line->palette();
    p.setBrush(QPalette::Base, p.brush(QPalette::ToolTipBase));
    ui->line->setPalette(p);

    m_timer->start();
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
                bool ate = true;

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

                    default:
                        ate = false;
                    break;
                }

                if(ate)
                    return true;
            }
            break; // event type

            default:
            break;
        }
    }

    return QWidget::eventFilter(watched, e);
}

void TickerMiniInput::slotRevertPalette()
{
    ui->line->setPalette(QApplication::palette(ui->line->metaObject()->className()));
}
