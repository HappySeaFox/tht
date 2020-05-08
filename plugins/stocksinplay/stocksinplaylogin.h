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

#ifndef STOCKSINPLAYLOGIN_H
#define STOCKSINPLAYLOGIN_H

#include <QStringList>

#include "networkaccessdialog.h"

class StocksInPlayLoginCookieJar;

class StocksInPlayLogin : public NetworkAccessDialog
{
    Q_OBJECT

public:
    explicit StocksInPlayLogin(const QString &login, const QString &password, QWidget *parent = 0);
    ~StocksInPlayLogin();

    QString hash() const;

protected:
    virtual bool finished();

private:
    QString m_id, m_hash;
    StocksInPlayLoginCookieJar *m_cookieJar;
};

inline
QString StocksInPlayLogin::hash() const
{
    return m_hash;
}

#endif // STOCKSINPLAYLOGIN_H
