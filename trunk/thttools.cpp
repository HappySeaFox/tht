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

#include <QApplication>
#include <QPainter>
#include <QWidget>
#include <QRegExp>
#include <QFile>
#include <QDir>

#include "styledescriptionreader.h"
#include "settings.h"
#include "thttools.h"
#include "target.h"

bool THTTools::m_isStyleApplied = false;

static const char * const THT_DEFAULT_STYLE =
// buttons in the ticker list
"QToolButton#pushList   { background: url(:/images/list.png)   center no-repeat; }"
"QToolButton#pushAdd    { background: url(:/images/add.png)    center no-repeat; }"
"QToolButton#pushSave   { background: url(:/images/save.png)   center no-repeat; }"
"QToolButton#pushExport { background: url(:/images/export.png) center no-repeat; }"

// "Links" button on the bottom
"QToolButton#pushLinkManager { background: url(:/images/links-load.png) center no-repeat; }"

// busy/not busy icons
"QLabel#labelBusy    { background: url(:/images/locked.png) center no-repeat; }"
"QLabel#labelNotBusy { background: url(:/images/ready.png)  center no-repeat; }"

THT_TARGET_DEFAULT_STYLESHEET
;

THTTools::THTTools()
{}

void THTTools::resetStyle(ResetStyleOnErrorType rt)
{
    QString style = SETTINGS_GET_STRING(SETTING_STYLE);

    qDebug("Style \"%s\"", qPrintable(style));

    if(!style.isEmpty())
    {
        const QString dirWithStyles = QDir::fromNativeSeparators(QCoreApplication::applicationDirPath() + QDir::separator()) + "styles";

        QFile file(dirWithStyles
                    + QDir::separator()
                    + style.replace(QRegExp("ini$"), "qss"));

        if(file.open(QIODevice::ReadOnly))
        {
            QString css = file.readAll();

            THTTools::m_isStyleApplied = true;

            qApp->setStyleSheet(THT_DEFAULT_STYLE + css
                                                    .replace("$SD", dirWithStyles)
                                );
        }
        else
        {
            qWarning("Style \"%s\" is not found, error: %s", qPrintable(style), qPrintable(file.errorString()));

            if(rt == ResetStyleOnError)
            {
                THTTools::m_isStyleApplied = false;
                qApp->setStyleSheet(QString());
            }
        }
    }
    else
    {
        THTTools::m_isStyleApplied = false;
        qApp->setStyleSheet(QString());
    }

    // notify widgets about style change
    QEvent ste(static_cast<QEvent::Type>(THT_STYLE_CHANGE_EVENT_TYPE));
    QApplication::sendEvent(qApp, &ste);
}

bool THTTools::isStyleApplied()
{
    return THTTools::m_isStyleApplied;
}

QPixmap THTTools::renderButtonWithPencil(QWidget *button, const QSize &size)
{
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);

    button->render(&p,
                   QPoint(0, 0),
                   QRegion((button->width() - size.width())/2,
                           (button->height() - size.height())/2,
                           size.width(),
                           size.height()),
                   QWidget::DrawChildren);

    static QPixmap pencil(":/images/pencil.png");

    p.drawPixmap(pixmap.width() - pencil.width(), pixmap.height() - pencil.height(), pencil);

    p.end();

    return pixmap;
}
