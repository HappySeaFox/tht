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

#ifndef NETWORKACCESS_H
#define NETWORKACCESS_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QSslError>
#include <QObject>
#include <QString>
#include <QList>

class QHttpMultiPart;
class QUrl;

class NetworkAccessPrivate;

/*
 *  Class to download/upload data and report the status
 */
class NetworkAccess : public QObject
{
    Q_OBJECT

public:
    explicit NetworkAccess(QObject *parent = 0);
    virtual ~NetworkAccess();

    /*
     *  Abort downloading. Doesn't emit any signal
     */
    void abort();

    /*
     *  The downloaded data. Call this after finished()
     */
    QByteArray data() const;

    /*
     *  Clear the internal buffer. data() will now return an empty array
     */
    void clearBuffer();

    /*
     *  Status of the network operation. Call this after finished()
     */
    QNetworkReply::NetworkError error() const;

    /*
     *  Network cookie JAR to use. If you need
     *  to send cookies, you need to call this method
     *  before get()
     */
    void setCookieJar(QNetworkCookieJar *cookieJar);

    /*
     *  Network cookie JAR in use
     */
    QNetworkCookieJar *cookieJar() const;

    /*
     *  Start a new network request. HEAD, GET, PUT and POST are supported
     */
    bool startRequest(QNetworkAccessManager::Operation operation,
                      const QNetworkRequest &request,
                      const QByteArray &data = QByteArray(), // for POST and PUT operations
                      QHttpMultiPart *multiPart = 0);

signals:
    /*
     *  The network operation is done. Call error() to check for errors
     */
    void finished();

    /*
     *  Same as QNetworkReply::downloadProgress()
     */
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    /*
     *  Same as QNetworkReply::uploadProgress()
     */
    void uploadProgress(qint64 bytesReceived, qint64 bytesTotal);

protected slots:
    /*
     *  You can reimplement this to do your own SSL checking.
     *  By default the following SSL errors are ignored:
     *      * QSslError::SelfSignedCertificate
     *      * QSslError::SelfSignedCertificateInChain
     *
     */
    virtual void slotSslErrors(const QList<QSslError> &errors);

private slots:
    void slotNetworkError(QNetworkReply::NetworkError);
    void slotNetworkData();
    void slotNetworkDone();

private:
    NetworkAccessPrivate *d;
};

#endif // NETWORKACCESS_H
