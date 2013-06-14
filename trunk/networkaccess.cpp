/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QPointer>
#include <QUrl>

#include <windows.h>

#include "networkaccess.h"

#ifdef NVER1
#include "settings.h"
#endif

class NetworkAccessPrivate
{
public:
    NetworkAccessPrivate()
    {
        reply = 0;
        error = QNetworkReply::NoError;
    }

    QNetworkReply::NetworkError error;
    QNetworkAccessManager *manager;
    QPointer<QNetworkReply> reply;
    QByteArray data;
};

/*******************************************/

NetworkAccess::NetworkAccess(QObject *parent) :
    QObject(parent)
{
    d = new NetworkAccessPrivate;

    d->manager = new QNetworkAccessManager(this);
}

NetworkAccess::~NetworkAccess()
{
    abort();

    delete d;
}

void NetworkAccess::get(const QUrl &url)
{
    abort();

    d->error = QNetworkReply::NoError;
    d->data.clear();

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
                         .arg(version.dwMinorVersion).toLatin1());

    d->reply = d->manager->get(request);

    connect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotNetworkError(QNetworkReply::NetworkError)));
    connect(d->reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotSslErrors(QList<QSslError>)));
    connect(d->reply, SIGNAL(finished()), this, SLOT(slotNetworkDone()));
    connect(d->reply, SIGNAL(readyRead()), this, SLOT(slotNetworkData()));
}

void NetworkAccess::abort()
{
    if(d->reply)
    {
        d->reply->blockSignals(true);
        d->reply->abort();
        delete d->reply;
    }
}

QByteArray NetworkAccess::data() const
{
    return d->data;
}

void NetworkAccess::clearBuffer()
{
    d->data.clear();
}

QNetworkReply::NetworkError NetworkAccess::error() const
{
    return d->error;
}

void NetworkAccess::setCookieJar(QNetworkCookieJar *cookieJar)
{
    d->manager->setCookieJar(cookieJar);
}

void NetworkAccess::slotNetworkError(QNetworkReply::NetworkError err)
{
    d->error = err;

    qDebug("Network error #%d", err);
}

void NetworkAccess::slotSslErrors(const QList<QSslError> &errors)
{
    const QList<QSslError> allowed = QList<QSslError>()
                                        << QSslError::SelfSignedCertificate
                                        << QSslError::SelfSignedCertificateInChain;
    foreach(QSslError e, errors)
    {
        if(allowed.indexOf(e.error()) < 0)
            return;
    }

    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    if(reply)
        reply->ignoreSslErrors();
}

void NetworkAccess::slotNetworkData()
{
    d->data += d->reply->readAll();
}

void NetworkAccess::slotNetworkDone()
{
    qDebug("Network request done");

    QUrl redirect;

    if(d->reply->error() == QNetworkReply::NoError)
    {
        QVariant v = d->reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

        if(v.isValid())
        {
            redirect = v.toUrl();

            if(!redirect.isEmpty() && redirect.isRelative())
                redirect = d->reply->request().url().resolved(redirect);
        }
    }

    d->reply->deleteLater();
    d->reply = 0;

    if(redirect.isEmpty())
        emit finished();
    else
    {
        qDebug("Redirecting");
        get(redirect);
    }
}
