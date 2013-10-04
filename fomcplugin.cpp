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

#include <QDesktopServices>
#include <QPalette>
#include <QLayout>
#include <QTimer>
#include <QFont>
#include <QUrl>

#include "clickablelabel.h"
#include "remotedate.h"
#include "fomcplugin.h"
#include "sqltools.h"

FomcPlugin::FomcPlugin() :
    Plugin(),
    m_timerFomcCheck(0),
    m_newYorkDate(0),
    m_label(0)
{
    THT_PLUGIN_INTERFACE_IMPLEMENTATION
}

FomcPlugin::~FomcPlugin()
{
    delete m_newYorkDate;
    delete m_label;
}

bool FomcPlugin::init()
{
    m_label = new ClickableLabel;

    if(!m_label)
    {
        qWarning("Cannot create ClickableLabel object");
        return false;
    }

    //: Message displayed to the user. FOMC - Federal Open Market Committee. It's ok not to translate "FOMC" (e.g. you can just copy-paste "FOMC" to your translation)
    m_label->setToolTip(tr("News from FOMC is today (click to open)"));
    //: FOMC - Federal Open Market Committee. It's ok not to translate "FOMC" (e.g. you can just copy-paste "FOMC" to your translation)
    m_label->setText(tr("FOMC"));

    QFont f = m_label->font();
    f.setBold(true);
    m_label->setFont(f);

    QPalette pal = m_label->palette();
    pal.setColor(QPalette::WindowText, Qt::red);
    m_label->setPalette(pal);

    connect(m_label, SIGNAL(clicked()), this, SLOT(slotClicked()));

    m_newYorkDate = new RemoteDate("Eastern Standard Time");

    if(!m_newYorkDate)
    {
        qWarning("Cannot create RemoteDate object");
        return false;
    }

    m_timerFomcCheck = new QTimer(this);

    if(!m_timerFomcCheck)
    {
        qWarning("Cannot create QTimer object");
        return false;
    }

    m_timerFomcCheck->setSingleShot(true);
    m_timerFomcCheck->setInterval(60*60*1000); // check every 1 hour
    connect(m_timerFomcCheck, SIGNAL(timeout()), this, SLOT(slotCheck()));

    return true;
}

void FomcPlugin::delayedInit()
{
    QWidget *c = containerRight();

    if(!c)
        return;

    m_label->setParent(c);
    c->layout()->addWidget(m_label);
    m_label->hide();

    QTimer::singleShot(0, this, SLOT(slotCheck()));
}

void FomcPlugin::slotCheck()
{
    // start check timer again
    m_timerFomcCheck->start();

    // determine the New York time
    QDateTime datetime = m_newYorkDate->dateTime();

    if(datetime.isValid())
    {
        qDebug("New York time: %s", qPrintable(datetime.toString("dd.MM.yyyy hh:mm:ss")));

        // query FOMC date
        QString date = datetime.toString("yyyy MM dd");
        QList<QVariantList> lists = SqlTools::query("SELECT date FROM fomc WHERE date = :date", ":date", date);

        if(!lists.isEmpty() && lists.at(0).size() == 1)
        {
            qDebug("News from FOMC is today");
            m_label->show();
            return;
        }
        else
            qDebug("Cannot query FOMC date");
    }
    else
        qDebug("New York time is invalid");

    m_label->hide();
}

void FomcPlugin::slotClicked()
{
    QDesktopServices::openUrl(QUrl("http://www.bloomberg.com/markets/economic-calendar"));
}

THT_PLUGIN_CONSTRUCTOR(FomcPlugin)
