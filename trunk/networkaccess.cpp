#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>

#include "networkaccess.h"
#include "settings.h"

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

    qDebug("Starting a new network request for \"%s\"", qPrintable(url.toString()));

    QNetworkRequest request(url);

    const OSVERSIONINFO version = Settings::instance()->version();

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

    emit finished();
}
