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

#ifndef STOCKSINPLAYDOWNLOADER_H
#define STOCKSINPLAYDOWNLOADER_H

#include <QStringList>
#include <QRegExp>
#include <QDialog>

#include "networkaccessdialog.h"

class QUrl;

class NetworkAccess;

class StocksInPlayDownloader : public NetworkAccessDialog
{
    Q_OBJECT

public:
    explicit StocksInPlayDownloader(const QUrl &url, QWidget *parent = 0);
    ~StocksInPlayDownloader();

    QStringList tickers() const;

protected:
    virtual bool finished();

private:
    QStringList m_tickers;
    enum Step { Page, Tickers };
    Step m_step;
    QRegExp m_rxBase64Request;
};

inline
QStringList StocksInPlayDownloader::tickers() const
{
    return m_tickers;
}

#endif // STOCKSINPLAYDOWNLOADER_H
