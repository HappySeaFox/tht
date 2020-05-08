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
#include <QByteArray>
#include <QUrl>

#include "stocksinplaylogincookiejar.h"
#include "stocksinplaytools.h"
#include "stocksinplaylogin.h"

StocksInPlayLogin::StocksInPlayLogin(const QString &login, const QString &password, QWidget *parent) :
    NetworkAccessDialog(parent)
{
    setWindowTitle(StocksInPlayTools::stocksInPlayTitle());
    setMessage(tr("Accessing..."));

    m_cookieJar = new StocksInPlayLoginCookieJar(this);
    setCookieJar(m_cookieJar);

    QNetworkRequest request(QUrl("http://" STOCKSINPLAY "/login.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    startRequest(QNetworkAccessManager::PostOperation,
                 request,
                 QByteArray("login=" + QUrl::toPercentEncoding(login) + '&'
                            + "password=" + QUrl::toPercentEncoding(password) + '&'
                            + "remember=on&"));
}

StocksInPlayLogin::~StocksInPlayLogin()
{}

bool StocksInPlayLogin::finished()
{
    m_hash = m_cookieJar->hash();

    if(m_hash.isEmpty())
    {
        showError(tr("Failed to get account details"));
        return false;
    }

    return true;
}
