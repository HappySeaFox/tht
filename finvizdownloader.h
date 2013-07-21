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

#ifndef FINVIZDOWNLOADER_H
#define FINVIZDOWNLOADER_H

#include <QStringList>
#include <QDialog>

#include "datadownloader.h"

class QUrl;

class NetworkAccess;

class FinvizDownloader : public DataDownloader
{
    Q_OBJECT

public:
    explicit FinvizDownloader(const QUrl &url, QWidget *parent = 0);
    ~FinvizDownloader();

    QStringList tickers() const;

protected:
    virtual bool finished();

private:
    QStringList m_tickers;
};

inline
QStringList FinvizDownloader::tickers() const
{
    return m_tickers;
}

#endif // FINVIZDOWNLOADER_H
