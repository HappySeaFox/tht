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

#include <QNetworkCookieJar>
#include <QUrl>

#include "datadownloader.h"
#include "networkaccess.h"
#include "ui_datadownloader.h"

class DataDownloaderPrivate
{
public:
    Ui::DataDownloader *ui;
    NetworkAccess *net;
    QStringList data;
};

/******************************************/

DataDownloader::DataDownloader(QWidget *parent) :
    QDialog(parent)
{
    d = new DataDownloaderPrivate;

    d->ui = new Ui::DataDownloader;
    d->ui->setupUi(this);

    d->ui->pushClose->hide();

    d->net = new NetworkAccess(this);

    connect(d->net, SIGNAL(finished()), this, SLOT(slotFinished()));
}

DataDownloader::~DataDownloader()
{
    delete d->ui;
    delete d;
}

void DataDownloader::setCookieJar(QNetworkCookieJar *jar)
{
    d->net->setCookieJar(jar);
}

void DataDownloader::get(const QUrl &url)
{
    d->net->get(url);
}

void DataDownloader::setMessage(const QString &m)
{
    d->ui->label->setText(m);
}

QString DataDownloader::data() const
{
    return d->net->data();
}

void DataDownloader::showError(const QString &e)
{
    d->ui->label->setText(e);
    d->ui->pushClose->show();
    d->ui->progressBar->setRange(0, 1);
    d->ui->progressBar->setValue(1);
}

void DataDownloader::slotFinished()
{
    if(d->net->error() != QNetworkReply::NoError)
    {
        showError(tr("Network error #%1").arg(d->net->error()));
        return;
    }

    finished();
    accept();
}
