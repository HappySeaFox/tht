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
#include "inlinetextinput.h"

#include "ui_inlinetextinput.h"

InlineTextInput::InlineTextInput(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InlineTextInput),
    m_active(false)
{
    ui->setupUi(this);

    ui->pushClose->setFocusProxy(ui->line);
    setFocusProxy(ui->line);

    connect(ui->pushClose, SIGNAL(clicked()), this, SIGNAL(cancel()));
    connect(ui->line, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));

    ui->line->installEventFilter(this);
}

InlineTextInput::~InlineTextInput()
{
    delete ui;
}

void InlineTextInput::startEditing(const QString &t)
{
    m_active = true;
    ui->line->setText(t);
}

QString InlineTextInput::text() const
{
    return ui->line->text();
}

void InlineTextInput::setUseTickerValidator(bool use)
{
    if(use)
        ui->line->setValidator(new UpperCaseValidator(ui->line));
    else
    {
        ui->line->setValidator(0);
        ui->line->setMaxLength(64);
    }
}

void InlineTextInput::setMaximumLength(int ml)
{
    ui->line->setMaxLength(ml);
}

bool InlineTextInput::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == ui->line)
    {
        bool doClose = false;
        bool doAccept = false;

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
                    case Qt::Key_Return:
                    case Qt::Key_Enter:
                        doAccept = true;
                    case Qt::Key_Escape:
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
            if(doAccept)
                emit accept();
            else
                emit cancel();

            return true;
        }
    }

    return QWidget::eventFilter(watched, e);
}
