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

#ifndef DROPBOXUPLOADER_H
#define DROPBOXUPLOADER_H

#include <QByteArray>
#include <QDialog>

#include "qdropbox.h"

namespace Ui
{
    class DropBoxUploader;
}

class DropBoxUploader : public QDialog
{
    Q_OBJECT

public:
    explicit DropBoxUploader(const QString &fileName, const QByteArray &binary, QWidget *parent = 0);
    ~DropBoxUploader();

    bool needRestart() const;

private:
    QString errorToString(QDropbox::Error);
    void message(const QString &text);
    void showError(const QString &text);
    void stopProgress();

private slots:
    void slotRequestTokenFinished(const QString &token, const QString &secret);
    void slotAccessTokenFinished(const QString &token, const QString &secret);
    void slotSharedLinkReceived(const QString &sharedLinkJson);
    void slotTokenExpired();
    void slotErrorOccured(QDropbox::Error);
    void slotDelayedTokenRequest();
    void slotDelayedAccessTokenRequest();
    void slotDelayedWork();

private:
    Ui::DropBoxUploader *ui;
    QString m_fileName;
    QByteArray m_binary;
    enum State { RequestingToken, RequestingAccessToken, RequestingSharedLink, Work };
    State m_state;
    QString m_token, m_tokenSecret, m_accessToken, m_accessTokenSecret;
    QDropbox *m_api;
    bool m_needRestart;
};

inline
bool DropBoxUploader::needRestart() const
{
    return m_needRestart;
}

#endif // DROPBOXUPLOADER_H
