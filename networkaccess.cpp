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

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>

#include <windows.h>

#include "finvizcookiejar.h"
#include "networkaccess.h"

#ifdef NVER1
#include "settings.h"
#endif

NetworkAccess::NetworkAccess(QObject *parent) :
    QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    m_manager->setCookieJar(new FinvizCookieJar(m_manager));

    m_reply = 0;
    m_error = QNetworkReply::NoError;
}

NetworkAccess::~NetworkAccess()
{
    abort();
}

void NetworkAccess::get(const QUrl &url)
{
    abort();

    m_error = QNetworkReply::NoError;
    m_data.clear();

    qDebug("Starting a new network request for \"%s\"", qPrintable(url.toString(QUrl::RemovePassword)));

    QNetworkRequest request(url);
    OSVERSIONINFO version;

// THT or TickersDb
#ifdef NVER1
    version = Settings::instance()->windowsVersion();
#else
    version.dwMajorVersion = 5;
    version.dwMinorVersion = 1;
    version.dwPlatformId = VER_PLATFORM_WIN32_NT;
#endif

    request.setRawHeader("Dnt", "1");
    request.setRawHeader("User-Agent", QString("Mozilla/5.0 (%1 %2.%3; rv:10.0) Gecko/20100101 Firefox/10.0")
                         .arg(version.dwPlatformId == VER_PLATFORM_WIN32_NT ? "Windows NT" : "Windows")
                         .arg(version.dwMajorVersion)
                         .arg(version.dwMinorVersion).toAscii());

    m_reply = m_manager->get(request);

    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotNetworkError(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(slotNetworkDone()));
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(slotNetworkData()));
}

void NetworkAccess::abort()
{
    if(m_reply)
    {
        m_reply->blockSignals(true);
        m_reply->abort();
        delete m_reply;
    }
}

void NetworkAccess::slotNetworkError(QNetworkReply::NetworkError err)
{
    m_error = err;

    qDebug("Network error #%d", err);
}

void NetworkAccess::slotNetworkData()
{
    m_data += m_reply->readAll();
}

void NetworkAccess::slotNetworkDone()
{
    qDebug("Network request done");

    QUrl redirect;

    if(m_reply->error() == QNetworkReply::NoError)
    {
        QVariant v = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

        if(v.isValid())
        {
            redirect = v.toUrl();

            if(!redirect.isEmpty() && redirect.isRelative())
                redirect = m_reply->request().url().resolved(redirect);
        }
    }

    m_reply->deleteLater();
    m_reply = 0;

    if(redirect.isEmpty())
        emit finished();
    else
    {
        qDebug("Redirecting");
        get(redirect);
    }
}
