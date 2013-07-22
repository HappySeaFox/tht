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

/*
 *  Dialog to download data and to show the result
 *  of the download operation
 *
 *  Steps to use:
 *      1) subclass DataDownloader
 *      2) implement finished()
 *
 *  For example:
 *
 *      class MyDownloader : public
 *      {
 *          ...
 *      public:
 *          QString data() const
 *          {
 *              return m_data;
 *          }
 *
 *      protected:
 *          virtual bool finished();
 *      };
 *
 *      bool MyDownloader::finished()
 *      {
 *          QString d = data();
 *
 *          if(!d.startsWith("XXX"))
 *          {
 *              showError(tr("Broken data, it doesn't start with 'XXX'"));
 *              return false;
 *          }
 *
 *          m_data = d.right(d.length() - 3);
 *
 *          return true;
 *      }
 */
class DataDownloader : public QDialog
{
    Q_OBJECT

public:
    explicit DataDownloader(QWidget *parent = 0);
    virtual ~DataDownloader();

protected:
    /*
     *  All the downloaded data. You call this method
     *  in finished()
     */
    QString data() const;

    /*
     *  Network cookie JAR to use. If you need
     *  to send cookies, you need to call this method
     *  before get()
     */
    void setCookieJar(QNetworkCookieJar *);

    /*
     *  Start downloading from the specified URL
     */
    void get(const QUrl &);

    /*
     *  Set the initial message. For example,
     *  "Downloading tickers..."
     */
    void setMessage(const QString &);

    /*
     *  Show and error message
     */
    void showError(const QString &);

    /*
     *  Subclasses must implement this. This method
     *  will be called when the download operation is finished.
     *  You can call data() and parse the downloaded data. If you encountered
     *  any error, return 'false'. Return 'true' on success
     */
    virtual bool finished() = 0;

private slots:
    void slotFinished();

private:
    DataDownloaderPrivate *d;
};

#endif // DATADOWNLOADER_H
