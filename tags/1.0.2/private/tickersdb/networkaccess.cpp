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

#include "networkaccess.h"

NetworkAccess::NetworkAccess(QObject *parent) :
    QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    m_reply = 0;
    m_error = QNetworkReply::NoError;

    connect(m_manager, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> &)),
            this, SLOT(slotSslErrors(QNetworkReply*, const QList<QSslError> &)));
}

NetworkAccess::~NetworkAccess()
{
    deleteReply();
}

void NetworkAccess::get(const QUrl &url)
{
    deleteReply();

    m_error = QNetworkReply::NoError;
    m_data.clear();

    qDebug("Starting a new network request for \"%s\"", qPrintable(url.toString(QUrl::RemovePassword)));

    QNetworkRequest request(url);

    request.setRawHeader("Dnt", "1");
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 5.1; rv:10.0) Gecko/20100101 Firefox/10.0");

    m_reply = m_manager->get(request);

    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotNetworkError(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(slotNetworkDone()));
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(slotNetworkData()));
}

void NetworkAccess::deleteReply()
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

void NetworkAccess::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &list)
{
    // allow self-signed certificates
    foreach(QSslError error, list)
    {
        if(error.error() != QSslError::SelfSignedCertificate
                && error.error() != QSslError::SelfSignedCertificateInChain)
            return;
    }

    if(reply)
        reply->ignoreSslErrors();
}

void NetworkAccess::slotNetworkData()
{
    m_data += m_reply->readAll();
}

void NetworkAccess::slotNetworkDone()
{
    qDebug("Network request done");

    m_reply->deleteLater();
    m_reply = 0;

    emit finished();
}
