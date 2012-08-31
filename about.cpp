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
#include <QShortcut>
#include <QTimer>
#include <QMovie>

#include "updatechecker.h"
#include "settings.h"
#include "about.h"

#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About),
    m_showExt(true)
{
    ui->setupUi(this);

    ui->label->setPixmap(QIcon(":/images/chart.ico").pixmap(48, 48));
    ui->labelVersion->setText(QString("THT %1").arg(NVER_STRING));

    const char * const http = HTTPROOT "/wiki/howto";
    ui->labelUrl->setText(QString("<a href=\"%1\">%2</a>").arg(http).arg(http));

    // set "wait" icon
    m_timer = new QTimer(this);

    m_timer->setSingleShot(true);
    m_timer->setInterval(750);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotSetWaitIcon()));

    // extension
    QLabel *label = new QLabel(QString(
                                   "<table align=center>"
                                   "<tr><td align=right><b>%1: </b></td><td>%2</td></tr>"
                                   "<tr><td align=right><b>%3: </b></td><td>%4</td></tr>"
                                   "</table>"
                                   )
                               .arg(tr("Persistent database"))
                               .arg(Settings::instance()->persistentDatabaseTimestamp().toString(Settings::instance()->databaseTimestampFormat()))
                               .arg(tr("Downloadable database"))
                               .arg(Settings::instance()->mutableDatabaseTimestamp().toString(Settings::instance()->databaseTimestampFormat()))
                               );

    label->setContentsMargins(0, 0, 0, 12);

    setOrientation(Qt::Vertical);
    setExtension(label);

    new QShortcut(QKeySequence::HelpContents, this, SLOT(slotExtendedAbout()));

    // update checker
    m_checker = new UpdateChecker(this);

    connect(m_checker, SIGNAL(newVersion(const QString &)), this, SLOT(slotNewVersion(const QString &)));
    connect(m_checker, SIGNAL(error(const QString &)), this, SLOT(slotError(const QString &)));

    m_checker->start();
    m_timer->start();
}

About::~About()
{
    delete ui;
}

void About::slotNewVersion(const QString &newVersion)
{
    QString tooltip;
    QString text;

    m_timer->stop();

    if(!newVersion.isEmpty())
    {
        text = "<html><head><meta name=\"qrichtext\" content=\"1\" /></head>"
                "<body><a href=\"" HTTPROOT "/downloads/list\">"
                    "<img src=\":/images/update.png\"></img>"
                "</a>"
                "</body></html>";
        tooltip = tr("Update available");
    }
    else
    {
        text = "<html><head><meta name=\"qrichtext\" content=\"1\" /></head>"
                "<body>"
                "<img src=\":/images/ok.png\"></img>"
                "</body></html>";
        tooltip = tr("THT is up to date");
    }

    ui->labelUpdate->setText(text);
    ui->labelUpdate->setToolTip(tooltip);
}

void About::slotError(const QString &err)
{
    m_timer->stop();

    ui->labelUpdate->setPixmap(QPixmap(":/images/error.png"));
    ui->labelUpdate->setToolTip(tr("Cannot check for updates (%1)").arg(err));
}

void About::slotExtendedAbout()
{
    showExtension(m_showExt);
    m_showExt = !m_showExt;
}

void About::slotSetWaitIcon()
{
    QMovie *movie = new QMovie(":/images/wait.gif", "GIF", this);
    ui->labelUpdate->setMovie(movie);
    movie->start();

    ui->labelUpdate->setToolTip(tr("Checking for updates..."));
}
