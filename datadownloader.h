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

#ifndef DATADOWNLOADER_H
#define DATADOWNLOADER_H

#include <QDialog>

class QNetworkCookieJar;
class QUrl;

class DataDownloaderPrivate;

class DataDownloader : public QDialog
{
    Q_OBJECT

public:
    explicit DataDownloader(QWidget *parent = 0);
    virtual ~DataDownloader();

protected:
    QString data() const;

    void setCookieJar(QNetworkCookieJar *);

    void get(const QUrl &);

    void setMessage(const QString &);

    void showError(const QString &);

    virtual void finished() = 0;

private slots:
    void slotFinished();

private:
    DataDownloaderPrivate *d;
};

#endif // DATADOWNLOADER_H
