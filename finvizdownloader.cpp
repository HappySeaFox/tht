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

#include <QMutableListIterator>
#include <QCloseEvent>
#include <QByteArray>
#include <QUrl>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QUrlQuery>
#endif

#include "finvizdownloader.h"
#include "finvizcookiejar.h"
#include "networkaccess.h"
#include "csvreader.h"
#include "ui_finvizdownloader.h"

static const int REQUIRED_FIELDS = 11;

FinvizDownloader::FinvizDownloader(const QUrl &url, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FinvizDownloader),
    m_allowClose(false)
{
    ui->setupUi(this);

    ui->pushClose->hide();

    m_net = new NetworkAccess(this);

    m_net->setCookieJar(new FinvizCookieJar(m_net));

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));

    QUrl u = url;

    // fix path
    u.setPath(u.path().replace("screener.ashx", "export.ashx"));

    QList<QPair<QString, QString> > items;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QUrlQuery uq(u);
    items = uq.queryItems();
#else
    items = u.queryItems();
#endif

    QMutableListIterator<QPair<QString, QString> > i(items);

    // fix "v" query item
    while(i.hasNext())
    {
        QPair<QString, QString> pair = i.next();

        if(pair.first == "v")
        {
            if(pair.second != "151")
            {
                pair.second = "151";
                i.setValue(pair);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                uq.setQueryItems(items);
#else
                u.setQueryItems(items);
#endif
            }

            break;
        }
    }

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    u.setQuery(uq);
#endif

    // download tickers as CSV
    m_net->get(u);
}

FinvizDownloader::~FinvizDownloader()
{
    delete ui;
}

void FinvizDownloader::closeEvent(QCloseEvent *e)
{
    if(m_allowClose)
        e->accept();
    else
        e->ignore();
}

void FinvizDownloader::showError(const QString &e)
{
    ui->label->setText(e);
    ui->pushClose->show();
    ui->progressBar->setRange(0, 1);
    ui->progressBar->setValue(1);
    m_allowClose = true;
}

void FinvizDownloader::slotFinished()
{
    if(m_net->error() != QNetworkReply::NoError)
    {
        showError(tr("Network error #%1").arg(m_net->error()));
        return;
    }

    // parse CSV
    CsvReader csv(m_net->data());

    QStringList str;
    csv.parseLine();

    while(!(str = csv.parseLine()).isEmpty())
    {
        if(str.size() != REQUIRED_FIELDS)
        {
            showError(tr("Broken data (fields: %1, required: %2)").arg(str.size()).arg(REQUIRED_FIELDS));
            return;
        }

        m_tickers.append(str[1]);
    }

    accept();
}
