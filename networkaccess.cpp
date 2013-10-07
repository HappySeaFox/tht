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
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QNetworkRequest>
#include <QPointer>
#include <QDebug>
#include <QUrl>

#include "networkaccess.h"

class NetworkAccessPrivate
{
public:
    NetworkAccessPrivate()
    {
        error = QNetworkReply::NoError;
        manager = 0;
        reply = 0;
        requestOperation = QNetworkAccessManager::CustomOperation;
        requestMultiPart = 0;
    }

    QNetworkReply::NetworkError error;
    QNetworkAccessManager *manager;
    QPointer<QNetworkReply> reply;
    QByteArray data;
    QNetworkAccessManager::Operation requestOperation;
    QNetworkRequest requestItself;
    QByteArray requestData;
    QHttpMultiPart *requestMultiPart;
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

void NetworkAccess::abort()
{
    if(d->reply)
    {
        d->reply->blockSignals(true);
        d->reply->abort();
        d->reply->deleteLater();
        d->reply = 0;
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

void NetworkAccess::startRequest(QNetworkAccessManager::Operation operation,
                                 const QNetworkRequest &request,
                                 const QByteArray &data,
                                 QHttpMultiPart *multiPart)
{
    abort();

    d->error = QNetworkReply::NoError;
    d->data.clear();

    qDebug("Starting a new network request for \"%s\"", qPrintable(request.url().toString(QUrl::RemovePassword)));

    QNetworkRequest rq(request);

    rq.setRawHeader("Dnt", "1");
    rq.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT rv:24.0) Gecko/20100101 Firefox/24.0");

    switch(operation)
    {
        case QNetworkAccessManager::HeadOperation:
             d->reply = d->manager->head(rq);
        break;

        case QNetworkAccessManager::GetOperation:
             d->reply = d->manager->get(rq);
        break;

        case QNetworkAccessManager::PutOperation:
        {
            if(multiPart)
                d->reply = d->manager->put(rq, multiPart);
            else
                d->reply = d->manager->put(rq, data);
        }
        break;

        case QNetworkAccessManager::PostOperation:
        {
            if(multiPart)
                d->reply = d->manager->post(rq, multiPart);
            else
                d->reply = d->manager->post(rq, data);
        }
        break;

        default:
        break;
    }

    if(!d->reply)
        return;

    // cache data
    d->requestOperation = operation;
    d->requestItself = rq;
    d->requestData = data;
    d->requestMultiPart = multiPart;

    connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(downloadProgress(qint64,qint64)));
    connect(d->reply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(uploadProgress(qint64,qint64)));

    connect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotNetworkError(QNetworkReply::NetworkError)));
    connect(d->reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotSslErrors(QList<QSslError>)));
    connect(d->reply, SIGNAL(finished()), this, SLOT(slotNetworkDone()));
    connect(d->reply, SIGNAL(readyRead()), this, SLOT(slotNetworkData()));
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

        QNetworkRequest rq = d->requestItself;

        rq.setUrl(redirect.isRelative() ? rq.url().resolved(redirect) : redirect);

        startRequest(d->requestOperation, rq, d->requestData, d->requestMultiPart);
    }
}
