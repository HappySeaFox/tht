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
#include "defaultstyles.h"
#include "listdetails.h"
#include "settings.h"
#include "thttools.h"
#include "target.h"

bool THTTools::m_isStyleApplied = false;

#define THT_DEFAULT_STYLESHEET_FOR_STYLE                                       \
"QWidget#stackListHeader, QWidget#stackListHeader > QWidget { border: none; }" \
"QToolButton#pushCloseInlineTextInput { background-color: transparent; }"      \
"QWidget#listContainer { border: none; }"                                      \
"QWidget#containerLeft { background-color: transparent; }"                     \
"QWidget#containerRight { background-color: transparent; }"                    \
"QLabel#target { background-color: transparent; }"                             \
"QLabel#numericLabel { background-color: transparent; }"                       \
"QToolButton#pushLinkManager { background-color: transparent; }"               \
"QWidget#stackBusy, QWidget#stackBusy * { background-color: transparent; }"

#define THT_DEFAULT_STYLE                  \
THT_BUSY_ICONS_DEFAULT_STYLESHEET          \
THT_LIST_BUTTONS_DEFAULT_STYLESHEET        \
THT_BUTTON_LINK_MANAGER_DEFAULT_STYLESHEET \
THT_TARGET_DEFAULT_STYLESHEET              \
THT_LIST_DETAILS_DEFAULT_STYLESHEET

THTTools::THTTools()
{}

void THTTools::resetStyle(ResetStyleOnErrorType rt)
{
    QString style = SETTINGS_GET_STRING(SETTING_STYLE);

    qDebug("Style \"%s\"", qPrintable(style));

    if(!style.isEmpty())
    {
        QString dirWithStyles = QDir::fromNativeSeparators(QCoreApplication::applicationDirPath() + QDir::separator()) + "styles";

        QFile file(dirWithStyles
                    + QDir::separator()
                    + style.replace(QRegExp("ini$"), "qss"));

        if(file.open(QIODevice::ReadOnly))
        {
            QString css = file.readAll();

            THTTools::m_isStyleApplied = true;

            qApp->setStyleSheet(THT_DEFAULT_STYLE
                                THT_DEFAULT_STYLESHEET_FOR_STYLE
                                + css.replace("$SD", dirWithStyles.replace('\'', "\\'"))
                                );
        }
        else
        {
            qWarning("Style \"%s\" is not found, error: %s", qPrintable(style), qPrintable(file.errorString()));

            if(rt == ResetStyleOnError)
            {
                THTTools::m_isStyleApplied = false;
                qApp->setStyleSheet(THT_DEFAULT_STYLE);
            }
        }
    }
    else
    {
        THTTools::m_isStyleApplied = false;
        qApp->setStyleSheet(THT_DEFAULT_STYLE);
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
