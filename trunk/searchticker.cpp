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

#include <QRegExpValidator>
#include <QKeySequence>
#include <QKeyEvent>
#include <QTimer>
#include <QEvent>

#include "uppercasevalidator.h"
#include "searchticker.h"

#include "ui_searchticker.h"

SearchTicker::SearchTicker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchTicker),
    m_active(false)
{
    ui->setupUi(this);

    ui->pushClose->setFocusProxy(ui->line);
    setFocusProxy(ui->line);

    ui->line->setValidator(new UpperCaseValidator(ui->line));

    connect(ui->pushClose, SIGNAL(clicked()), this, SIGNAL(cancel()));
    connect(ui->line, SIGNAL(textChanged(const QString &)), this, SIGNAL(ticker(const QString &)));

    ui->line->installEventFilter(this);
}

SearchTicker::~SearchTicker()
{
    delete ui;
}

void SearchTicker::startSearching()
{
    m_active = true;
    ui->line->clear();
}

bool SearchTicker::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == ui->line)
    {
        bool doClose = false;

        switch(e->type())
        {
            case QEvent::FocusOut:
                if(m_active)
                    doClose = true;
            break;

            case QEvent::KeyPress:
            {
                QKeyEvent *ke = static_cast<QKeyEvent *>(e);

                // search widget
                if(ke->matches(QKeySequence::FindNext))
                    emit next();
                else switch(ke->key())
                {
                    case Qt::Key_Escape:
                    case Qt::Key_Return:
                    case Qt::Key_Enter:
                        doClose = true;
                    break;
                }
            }
            break;

            default:
            break;
        }

        if(doClose)
        {
            emit cancel();
            return true;
        }
    }

    return QWidget::eventFilter(watched, e);
}
