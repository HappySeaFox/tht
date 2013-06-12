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

#ifndef NETWORKACCESS_H
#define NETWORKACCESS_H

#include <QNetworkReply>
#include <QByteArray>
#include <QSslError>
#include <QPointer>
#include <QObject>
#include <QString>
#include <QList>

class QNetworkAccessManager;
class QUrl;

class NetworkAccess : public QObject
{
    Q_OBJECT

public:
    explicit NetworkAccess(QObject *parent = 0);
    virtual ~NetworkAccess();

    void get(const QUrl &url);

    void abort();

    QByteArray data() const;

    void clearBuffer();

    QNetworkReply::NetworkError error() const;

    void setCookieJar(QNetworkCookieJar *cookieJar);

signals:
    void finished();

protected slots:
    virtual void slotSslErrors(const QList<QSslError> &errors);

private slots:
    void slotNetworkError(QNetworkReply::NetworkError);
    void slotNetworkData();
    void slotNetworkDone();

private:
    QNetworkReply::NetworkError m_error;
    QNetworkAccessManager *m_manager;
    QPointer<QNetworkReply> m_reply;
    QByteArray m_data;
};

inline
QNetworkReply::NetworkError NetworkAccess::error() const
{
    return m_error;
}

inline
QByteArray NetworkAccess::data() const
{
    return m_data;
}

inline
void NetworkAccess::clearBuffer()
{
    m_data.clear();
}

#endif // TICKERINFORMATIONFETCHER_H
