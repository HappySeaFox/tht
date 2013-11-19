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

#include <QNetworkRequest>
#include <QStringList>
#include <QRegExp>
#include <QTimer>

#include "networkaccess.h"
#include "updatechecker.h"
#include "tools.h"

UpdateChecker::UpdateChecker(QObject *parent) :
    QObject(parent)
{
    m_lastVersion = NVER_STRING;

    m_url = QUrl(SVNROOT_FOR_DOWNLOAD "/THT-version.tag?format=raw");

    m_rxVersion = QRegExp("^(\\d+)\\.(\\d+)\\.(\\d+)$");

    m_net = new NetworkAccess(this);

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));
}

void UpdateChecker::start()
{
    m_net->startRequest(QNetworkAccessManager::GetOperation, QNetworkRequest(m_url));
}

void UpdateChecker::slotFinished()
{
    if(m_net->error() != QNetworkReply::NoError)
    {
        emit error(Tools::networkErrorTitle().arg(m_net->error()));
        return;
    }

    QStringList list = QString(m_net->data()).split(QRegExp("\\r?\\n"), QString::SkipEmptyParts);

    if(list.isEmpty() || !m_rxVersion.exactMatch(list[0]))
    {
        qWarning("Update checker: answer is broken");
        emit error(tr("Server answer is broken"));
        return;
    }

    if(m_lastVersion != list[0])
    {
        m_lastVersion = list[0];

        bool okmajor, okminor, okpatch;

        int major = m_rxVersion.cap(1).toInt(&okmajor);
        int minor = m_rxVersion.cap(2).toInt(&okminor);
        int patch = m_rxVersion.cap(3).toInt(&okpatch);

        if(okmajor && okminor && okpatch)
        {
            if(major > NVER1
                    || (major == NVER1 && minor > NVER2)
                    || (major == NVER1 && minor == NVER2 && patch > NVER3)
                    )
            {
                qDebug("Update checker: new version is \"%s\"", qPrintable(m_lastVersion));
                emit newVersion(m_lastVersion);
            }
            else
            {
                qDebug("Update checker: current version is better than \"%s\"", qPrintable(m_lastVersion));
                emit newVersion(QString());
            }
        }
        else
        {
            qDebug("Update checker: new version is \"%s\"", qPrintable(m_lastVersion));
            emit newVersion(m_lastVersion);
        }
    }
    else
    {
        qDebug("Update checker: version is up-to-date");
        emit newVersion(QString());
    }
}
