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

#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QTimer>

#include <climits>

#include "networkaccessdialog.h"
#include "networkaccess.h"
#include "tools.h"
#include "ui_networkaccessdialog.h"

class NetworkAccessDialogPrivate
{
public:
    NetworkAccessDialogPrivate()
        : ui(0),
          net(0),
          delayedRequestTimer(0),
          requestOperation(QNetworkAccessManager::CustomOperation),
          requestMultiPart(0)
    {}

    Ui::NetworkAccessDialog *ui;
    NetworkAccess *net;
    QStringList data;
    QTimer *delayedRequestTimer;
    QNetworkAccessManager::Operation requestOperation;
    QNetworkRequest requestItself;
    QByteArray requestData;
    QHttpMultiPart *requestMultiPart;
};

/******************************************/

NetworkAccessDialog::NetworkAccessDialog(QWidget *parent) :
    QDialog(parent)
{
    d = new NetworkAccessDialogPrivate;

    d->delayedRequestTimer = new QTimer(this);
    d->delayedRequestTimer->setSingleShot(true);
    d->delayedRequestTimer->setInterval(0);
    connect(d->delayedRequestTimer, SIGNAL(timeout()), this, SLOT(slotDelayedRequest()));

    d->ui = new Ui::NetworkAccessDialog;
    d->ui->setupUi(this);

    d->ui->pushClose->hide();
}

NetworkAccessDialog::~NetworkAccessDialog()
{
    delete d->ui;
    delete d;
}

void NetworkAccessDialog::setCookieJar(QNetworkCookieJar *jar)
{
    ensureNetworkAccessIsCreated();
    d->net->setCookieJar(jar);
}

void NetworkAccessDialog::startRequest(QNetworkAccessManager::Operation operation, const QNetworkRequest &request, const QByteArray &data, QHttpMultiPart *multiPart)
{
    ensureNetworkAccessIsCreated();

    d->requestOperation = operation;
    d->requestItself = request;
    d->requestData = data;
    d->requestMultiPart = multiPart;

    d->delayedRequestTimer->start();
}

void NetworkAccessDialog::setMessage(const QString &m)
{
    d->ui->label->setText(m);
}

QByteArray NetworkAccessDialog::data() const
{
    return d->net ? d->net->data() : QByteArray();
}

QProgressBar *NetworkAccessDialog::progressBar() const
{
    return d->ui->progressBar;
}

void NetworkAccessDialog::setNetworkAccess(NetworkAccess *newNA)
{
    if(!d->net)
    {
        d->net = newNA;
        connectSignals();
    }
}

NetworkAccess *NetworkAccessDialog::networkAccess() const
{
    return d->net;
}

void NetworkAccessDialog::showError(const QString &e)
{
    d->ui->label->setText(e);
    d->ui->pushClose->show();
    d->ui->progressBar->setRange(0, 1);
    d->ui->progressBar->setValue(1);
}

bool NetworkAccessDialog::event(QEvent *e)
{
    return QDialog::event(e);
}

void NetworkAccessDialog::ensureNetworkAccessIsCreated()
{
    if(!d->net)
    {
        d->net = new NetworkAccess(this);
        connectSignals();
    }
}

void NetworkAccessDialog::connectSignals()
{
    connect(d->net, SIGNAL(finished()), this, SLOT(slotFinished()));
    connect(d->net, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(slotUpdateProgress(qint64,qint64)));
    connect(d->net, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(slotUpdateProgress(qint64,qint64)));
}

void NetworkAccessDialog::slotFinished()
{
    if(d->net->error() != QNetworkReply::NoError)
    {
        showError(Tools::networkErrorTitle().arg(d->net->error()));
        return;
    }

    if(finished())
        accept();
}

void NetworkAccessDialog::slotDelayedRequest()
{
    d->ui->progressBar->setRange(0, 0);
    d->ui->progressBar->setValue(-1);

    d->net->startRequest(d->requestOperation, d->requestItself, d->requestData, d->requestMultiPart);
}

void NetworkAccessDialog::slotUpdateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(bytesReceived > 0 && bytesTotal > 0)
    {
        // scale down. Support size up to 2 Tb
        if(bytesTotal > INT_MAX)
        {
            bytesTotal /= 1024;
            bytesReceived /= 1024;
        }

        // update range
        if(!d->ui->progressBar->maximum() && !d->ui->progressBar->minimum())
            d->ui->progressBar->setRange(0, bytesTotal);

        d->ui->progressBar->setValue(bytesReceived);
    }
}
