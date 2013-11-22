/*
 * This file is part of THT-lib.
 *
 * THT-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation.
 *
 * THT-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NETWORKACCESSDIALOG_H
#define NETWORKACCESSDIALOG_H

#include <QNetworkAccessManager>
#include <QByteArray>
#include <QDialog>

class QNetworkCookieJar;
class QNetworkRequest;
class QHttpMultiPart;
class QProgressBar;
class QUrl;

class NetworkAccessDialogPrivate;
class NetworkAccess;

/*
 *  Dialog to download data and to show the result
 *  of the download operation
 *
 *  Look'n'feel:
 *
 *  +----------------------------+
 *  | <message>                  |
 *  |                            |
 *  | ||||| progress ||| [close] |
 *  +----------------------------+
 *
 *  Steps to use:
 *      1) subclass NetworkAccessDialog
 *      2) implement finished()
 *
 *  For example:
 *
 *      class MyDownloader : public NetworkAccessDialog
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
 *              showError(tr("Broken answer, it doesn't start with 'XXX'"));
 *              return false;
 *          }
 *
 *          m_data = d.right(d.length() - 3);
 *
 *          return true;
 *      }
 */
class NetworkAccessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkAccessDialog(QWidget *parent = 0);
    virtual ~NetworkAccessDialog();

protected:
    /*
     *  All the downloaded data. You call this method
     *  in finished()
     */
    QByteArray data() const;

    /*
     *  Access to progress bar
     */
    QProgressBar *progressBar() const;

    /*
     *  Set your own NetworkAccess object to process network operations.
     *  Must be called before any network operation is fired. Otherwise
     *  newNA will be ignored
     */
    void setNetworkAccess(NetworkAccess *newNA);

    /*
     *  Network access object in use
     */
    NetworkAccess *networkAccess() const;

    /*
     *  Network cookie JAR to use. If you need
     *  to send cookies, you need to call this method
     *  before get()
     */
    void setCookieJar(QNetworkCookieJar *);

    /*
     *  Start a new network request
     */
    void startRequest(QNetworkAccessManager::Operation operation,
                      const QNetworkRequest &request,
                      const QByteArray &data = QByteArray(), // for POST and PUT operations
                      QHttpMultiPart *multiPart = 0);

    /*
     *  Show the message. For example,
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
     *  an error or would like to start another network operation, return 'false'.
     *  If you return 'true' the dialog will be closed with 'Accepted' result
     */
    virtual bool finished() = 0;

    virtual bool event(QEvent *e);

private:
    void ensureNetworkAccessIsCreated();
    void connectSignals();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);

private slots:
    void slotFinished();
    void slotDelayedRequest();
    void slotUpdateProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    NetworkAccessDialogPrivate *d;
};

#endif // NETWORKACCESSDIALOG_H
