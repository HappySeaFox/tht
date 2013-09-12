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

#include <QDesktopServices>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QUrl>

#include "qdropboxaccount.h"
#include "dropboxuploader.h"
#include "qdropboxfile.h"
#include "qdropboxjson.h"
#include "settings.h"
#include "ui_dropboxuploader.h"

DropBoxUploader::DropBoxUploader(const QString &fileName, const QByteArray &binary, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DropBoxUploader),
    m_fileName(fileName),
    m_binary(binary),
    m_state(NotInitialized)
{
    ui->setupUi(this);

    m_api = new QDropbox(this);

    m_api->setKey("r8gikc6qz954etm");
    m_api->setSharedSecret("l7txodl36k2rga2");

    connect(m_api, SIGNAL(requestTokenFinished(QString,QString)), this, SLOT(slotRequestTokenFinished(QString,QString)));
    connect(m_api, SIGNAL(accessTokenFinished(QString,QString)), this, SLOT(slotAccessTokenFinished(QString,QString)));
    connect(m_api, SIGNAL(sharedLinkReceived(QString)), this, SLOT(slotSharedLinkReceived(QString)));
    connect(m_api, SIGNAL(tokenExpired()), this, SLOT(slotTokenExpired()));
    connect(m_api, SIGNAL(errorOccured(QDropbox::Error)), this, SLOT(slotErrorOccured(QDropbox::Error)));

    QString t = SETTINGS_GET_STRING(SETTING_DROPBOX_TOKEN);

    if(t.isEmpty())
        QTimer::singleShot(0, this, SLOT(slotDelayedTokenRequest()));
    else
    {
        m_api->setToken(t);
        m_api->setTokenSecret(SETTINGS_GET_STRING(SETTING_DROPBOX_TOKEN_SECRET));

        t = SETTINGS_GET_STRING(SETTING_DROPBOX_ACCESS_TOKEN);

        if(t.isEmpty())
            QTimer::singleShot(0, this, SLOT(slotDelayedAccessTokenRequest()));
        else
        {
            m_api->setToken(t);
            m_api->setTokenSecret(SETTINGS_GET_STRING(SETTING_DROPBOX_ACCESS_TOKEN_SECRET));

            QTimer::singleShot(0, this, SLOT(slotDelayedWork()));
        }
    }
}

DropBoxUploader::~DropBoxUploader()
{
    delete ui;
}

QString DropBoxUploader::errorToString(QDropbox::Error code)
{
    switch(code)
    {
        case QDropbox::CommunicationError:       return tr("Network error");
        case QDropbox::VersionNotSupported:      return tr("Used API version is not supported");
        case QDropbox::UnknownAuthMethod:        return tr("Used authentication method is not supported");
        case QDropbox::ResponseToUnknownRequest: return tr("Unexpected response");
        case QDropbox::APIError:                 return tr("API error");
        case QDropbox::UnknownQueryMethod:       return tr("Unknown query method");
        case QDropbox::BadInput:                 return tr("Bad parameter");
        case QDropbox::BadOAuthRequest:          return tr("Bad oAuth request");
        case QDropbox::WrongHttpMethod:          return tr("Bad HTTP method");
        case QDropbox::MaxRequestsExceeded:      return tr("Maximum amount of requests exceeded");
        case QDropbox::UserOverQuota:            return tr("Quota exceeded");
        case QDropbox::TokenExpired:             return tr("Token has expired");

        default:
            return tr("Unknown error");
    }
}

void DropBoxUploader::message(const QString &text)
{
    ui->label->setText(text);
}

void DropBoxUploader::showError(const QString &text)
{
    qDebug("Error: %s", qPrintable(text));
    message(text);
    stopProgress();
}

void DropBoxUploader::stopProgress()
{
    ui->progress->setRange(0, 1);
    ui->progress->setValue(1);
}

void DropBoxUploader::slotRequestTokenFinished(const QString &token, const QString &secret)
{
    m_token = token;
    m_tokenSecret = secret;

    if(QMessageBox::information(this,
                                tr("Dropbox"),
                                tr("You need to authorize THT in your Dropbox account. Click 'OK' to open the link in browser. After that open this dialog again"),
                                QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
    {
        QDesktopServices::openUrl(m_api->authorizeLink());

        SETTINGS_SET_STRING(SETTING_DROPBOX_TOKEN, m_token, Settings::NoSync);
        SETTINGS_SET_STRING(SETTING_DROPBOX_TOKEN_SECRET, m_tokenSecret);

        m_needRestart = true;
    }

    reject();
}

void DropBoxUploader::slotAccessTokenFinished(const QString &token, const QString &secret)
{
    m_accessToken = token;
    m_accessTokenSecret = secret;

    SETTINGS_SET_STRING(SETTING_DROPBOX_ACCESS_TOKEN, m_accessToken, Settings::NoSync);
    SETTINGS_SET_STRING(SETTING_DROPBOX_ACCESS_TOKEN_SECRET, m_accessTokenSecret);

    m_api->setToken(m_accessToken);
    m_api->setTokenSecret(m_accessTokenSecret);

    QTimer::singleShot(0, this, SLOT(slotDelayedWork()));
}

void DropBoxUploader::slotSharedLinkReceived(const QString &sharedLinkJson)
{
    qDebug("Shared link recieved");

    QDropboxJson parser(sharedLinkJson);

    QString parsedLink = QUrl::fromPercentEncoding(parser.getString("url").toUtf8());

    if(parsedLink.isEmpty())
        showError(tr("Cannot get the link to the file"));
    else
        message(QString("<a href=\"%1\">%2</a>").arg(parsedLink).arg(parsedLink));
}

void DropBoxUploader::slotTokenExpired()
{
    slotErrorOccured(QDropbox::TokenExpired);
}

void DropBoxUploader::slotErrorOccured(QDropbox::Error error)
{
    qDebug("Dropbox error #%d in state %d", error, m_state);

    if(error == QDropbox::TokenExpired && m_state != RequestingToken)
        QTimer::singleShot(0, this, SLOT(slotDelayedTokenRequest()));
    else
        showError(errorToString(error));
}

void DropBoxUploader::slotDelayedTokenRequest()
{
    message(tr("Requesting new token..."));

    qDebug("Requesting new token");

    m_state = RequestingToken;

    SETTINGS_SET_STRING(SETTING_DROPBOX_TOKEN, QString(), Settings::NoSync);
    SETTINGS_SET_STRING(SETTING_DROPBOX_TOKEN_SECRET, QString(), Settings::NoSync);

    SETTINGS_SET_STRING(SETTING_DROPBOX_ACCESS_TOKEN, QString(), Settings::NoSync);
    SETTINGS_SET_STRING(SETTING_DROPBOX_ACCESS_TOKEN_SECRET, QString());

    m_api->setToken(QString());
    m_api->setTokenSecret(QString());

    m_api->requestToken();
}

void DropBoxUploader::slotDelayedAccessTokenRequest()
{
    message(tr("Requesting new access token..."));

    qDebug("Requesting new access token");

    m_state = RequestingAccessToken;

    SETTINGS_SET_STRING(SETTING_DROPBOX_ACCESS_TOKEN, QString(), Settings::NoSync);
    SETTINGS_SET_STRING(SETTING_DROPBOX_ACCESS_TOKEN_SECRET, QString());

    m_api->requestAccessToken();
}

void DropBoxUploader::slotDelayedWork()
{
    message(tr("Uploading..."));

    qDebug("Uploading to Dropbox");

    m_state = Work;

    QDropboxFile file("/sandbox/" + m_fileName, m_api);

    if(file.open(QIODevice::WriteOnly))
    {
        if(file.write(m_binary) != m_binary.size())
        {
            showError(tr("Cannot upload file (%1)").arg(file.errorString()));
            return;
        }

        file.close();

        message(tr("Requesting link..."));
        m_state = RequestingSharedLink;
        m_api->requestSharedLink(file.filename());
    }
    else
    {
        qDebug("Cannot open file: %s", qPrintable(file.errorString()));
        showError(tr("Cannot upload file (%1)").arg(file.errorString()));
        return;
    }

    stopProgress();

    //: Command
    ui->pushCancel->setText(tr("Close"));
    disconnect(ui->pushCancel, SIGNAL(clicked()), this, 0);
    connect(ui->pushCancel, SIGNAL(clicked()), this, SLOT(accept()));
}
