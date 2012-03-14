#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>
#include <QRegExp>
#include <QTimer>

#include "updatechecker.h"

UpdateChecker *UpdateChecker::instance()
{
    static UpdateChecker *m_inst = new UpdateChecker;

    return m_inst;
}

UpdateChecker::UpdateChecker() : QObject()
{
    m_lastVersion = NVER_STRING;

    m_rxVersion = QRegExp("^\\d+\\.\\d+\\.\\d+$");

    m_net = new QNetworkAccessManager(this);

    connect(m_net, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> &)),
            this, SLOT(slotSslErrors(QNetworkReply*, const QList<QSslError> &)));

    connect(m_net, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(slotFinished(QNetworkReply *)));

    startRequest();
}

void UpdateChecker::startRequest()
{
    m_net->get(QNetworkRequest(QUrl("http://traders-home-task-ng.googlecode.com/svn/trunk/THT-version.tag")));
}

void UpdateChecker::slotFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug("Update checker: Network error #%d (%s)", reply->error(), qPrintable(reply->errorString()));
        return;
    }

    QStringList list = QString(reply->readAll()).split(QRegExp("(\\r\\n|\\n)"), QString::SkipEmptyParts);

    if(list.isEmpty())
    {
        qWarning("Update checker: answer is empty");
        return;
    }

    if(!m_rxVersion.exactMatch(list[0]))
    {
        qWarning("Update checker: answer is broken");
        return;
    }

    if(m_lastVersion != list[0])
    {
        m_lastVersion = list[0];

        qDebug("Update checker: new version is \"%s\"", qPrintable(m_lastVersion));
        emit newVersion(m_lastVersion);

        // check every 3 hours
        QTimer::singleShot(3*3600*1000, this, SLOT(startRequest()));
    }
    else
        qDebug("Update checker: version is up-to-date");
}

void UpdateChecker::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &list)
{
    // allow self-signed certificates
    foreach(QSslError error, list)
    {
        if(error.error() != QSslError::SelfSignedCertificate
                && error.error() != QSslError::SelfSignedCertificateInChain)
            return;
    }

    reply->ignoreSslErrors();
}
