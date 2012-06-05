/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2012 by Dmitry 'Krasu' Baryshev                         *
 *   ksquirrel.iv@gmail.com                                                *
 ***************************************************************************/

// This TU looks like a crap

#include <QNetworkAccessManager>
#include <QWebElementCollection>
#include <QDesktopWidget>
#include <QTextDocument>
#include <QNetworkReply>
#include <QStylePainter>
#include <QApplication>
#include <QStyleOption>
#include <QWebSettings>
#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>
#include <QPointer>
#include <QEvent>
#include <QLabel>
#include <QStyle>
#include <QTimer>
#include <QDebug>
#include <QHash>

#include "tickerinformationtooltip.h"
#include "settings.h"

class TickerInformationToolTipLabel : public QLabel
{
    Q_OBJECT

public:
    TickerInformationToolTipLabel(const QString &text, bool ticker, QWidget *w);
    ~TickerInformationToolTipLabel();

    static TickerInformationToolTipLabel *instance;

    bool eventFilter(QObject *, QEvent *);

    QBasicTimer hideTimer, expireTimer;

    bool fadingOut;

    void reuseTip(const QString &text, bool isTicker);
    void hideTip();
    void hideTipImmediately();
    void restartExpireTimer();
    bool tipChanged(const QString &text);
    void placeTip(const QPoint &pos);

    static int getTipScreen(const QPoint &pos);

protected:
    void timerEvent(QTimerEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

public slots:
    void styleSheetParentDestroyed()
    {
        setProperty("_q_stylesheet_parent", QVariant());
        styleSheetParent = 0;
    }

private slots:
    void slotNetworkError(QNetworkReply::NetworkError);
    void slotNetworkDone();
    void slotNetworkData();

private:
    QWidget *styleSheetParent;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QString data;
    QPoint lastPos;
    QString ticker;
};

TickerInformationToolTipLabel *TickerInformationToolTipLabel::instance = 0;

TickerInformationToolTipLabel::TickerInformationToolTipLabel(const QString &text, bool ticker, QWidget *w)
    : QLabel(w, Qt::ToolTip | Qt::BypassGraphicsProxyWidget), styleSheetParent(0)
{
    delete instance;
    instance = this;

    manager = new QNetworkAccessManager(this);
    reply = 0;

    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
    setPalette(TickerInformationToolTip::palette());
    ensurePolished();
    setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this));
    setFrameStyle(QFrame::NoFrame);
    setAlignment(Qt::AlignLeft);
    setIndent(1);
    qApp->installEventFilter(this);
    setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this) / qreal(255.0));
    setMouseTracking(true);
    fadingOut = false;
    reuseTip(text, ticker);
}

void TickerInformationToolTipLabel::restartExpireTimer()
{
    int time = 10000 + 40 * qMax(0, text().length()-100);
    expireTimer.start(time, this);
    hideTimer.stop();
}

void TickerInformationToolTipLabel::reuseTip(const QString &text, bool isTicker)
{
    if(styleSheetParent)
    {
        disconnect(styleSheetParent, SIGNAL(destroyed()),
                   TickerInformationToolTipLabel::instance, SLOT(styleSheetParentDestroyed()));
        styleSheetParent = 0;
    }

    setWordWrap(Qt::mightBeRichText(text));
    setText(isTicker ? (text + "...") : text);

    QFontMetrics fm(font());
    QSize extra(1, 0);

    // Make it look good with the default ToolTip font on Mac, which has a small descent.
    if (fm.descent() == 2 && fm.ascent() >= 11)
        ++extra.rheight();

    resize(sizeHint() + extra);

    if(!isTicker)
        return;

    ticker = text;

    if(reply)
    {
        data.clear();
        reply->blockSignals(true);
        reply->abort();
        delete reply;
    }

    qDebug("Starting a new network request for \"%s\"", qPrintable(text));

    // http://finance.yahoo.com/q/in?s=A
    QNetworkRequest request(
                QUrl(QString("http://finance.yahoo.com/q/in?s=%1")
                     .arg(QString(text).replace('.', '-')))); // Yahoo requires '.' to be replaced with '-'

    const OSVERSIONINFO version = Settings::instance()->version();

    request.setRawHeader("Dnt", "1");
    request.setRawHeader("User-Agent", QString("Mozilla/5.0 (%1 %2.%3; rv:10.0) Gecko/20100101 Firefox/10.0")
                         .arg(version.dwPlatformId == VER_PLATFORM_WIN32_NT ? "Windows NT" : "Windows")
                         .arg(version.dwMajorVersion)
                         .arg(version.dwMinorVersion).toAscii());

    reply = manager->get(request);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotNetworkError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(slotNetworkDone()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(slotNetworkData()));
}

void TickerInformationToolTipLabel::slotNetworkError(QNetworkReply::NetworkError err)
{
    qDebug("Network error #%d", err);
}

void TickerInformationToolTipLabel::slotNetworkDone()
{
    qDebug("Network request done");

    if(reply->error() != QNetworkReply::NoError)
    {
        TickerInformationToolTip::showText(QPoint(),
                                           reply->error() == QNetworkReply::UnknownContentError
                                                ? tr("Not found")
                                                : tr("Error #%1").arg(reply->error()),
                                           false);
        restartExpireTimer();
        return;
    }

    QWebPage page;
    QString result, sector, industry;

    page.settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    page.settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    page.settings()->setAttribute(QWebSettings::JavaEnabled, false);
    page.settings()->setAttribute(QWebSettings::PluginsEnabled, false);

    page.mainFrame()->setHtml(data);

    // ticker name
    QWebElementCollection tables = page.mainFrame()->findAllElements("table");
    bool found = false;

    foreach(QWebElement table, tables)
    {
        QWebElement tr = table.findFirst("tr");
        QWebElement th1 = tr.findFirst("th.yfnc_tablehead1");

        if(th1.toPlainText() == "Name")
        {
            QWebElement th2 = th1.nextSibling();

            if(th2.toPlainText() == "Ticker")
            {
                result = tr.nextSibling().findFirst("td").findFirst("a").toPlainText();
                found = true;
                break;
            }
        }
    }

    if(result.isEmpty())
    {
        TickerInformationToolTip::showText(QPoint(), found ? tr("Parse error") : tr("Not found"), false);
        restartExpireTimer();
        return;
    }

    // sector & industry
    QWebElementCollection ths = page.mainFrame()->findAllElements("th.yfnc_tablehead1");

    foreach(QWebElement th, ths)
    {
        if(sector.isEmpty() && th.toPlainText() == "Sector:")
            sector = th.nextSibling().findFirst("a").toPlainText();

        if(industry.isEmpty() && th.toPlainText() == "Industry:")
            industry = th.nextSibling().findFirst("a").toPlainText();

        // everything is found
        if(!sector.isEmpty() && !industry.isEmpty())
            break;
    }

    // resulting tooltip
    if(!sector.isEmpty())
    {
        result += '\n' + sector;

        if(!industry.isEmpty())
            result += " / " + industry;
    }

    TickerInformationToolTip::showText(QPoint(), result, false);
    restartExpireTimer();
}

void TickerInformationToolTipLabel::slotNetworkData()
{
    data += reply->readAll();
}

void TickerInformationToolTipLabel::paintEvent(QPaintEvent *ev)
{
    QStylePainter p(this);
    QStyleOptionFrame opt;
    opt.init(this);
    p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
    p.end();

    QLabel::paintEvent(ev);
}

void TickerInformationToolTipLabel::resizeEvent(QResizeEvent *e)
{
    QStyleHintReturnMask frameMask;
    QStyleOption option;
    option.init(this);
    if (style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
        setMask(frameMask.region);

    QLabel::resizeEvent(e);
}

TickerInformationToolTipLabel::~TickerInformationToolTipLabel()
{
    instance = 0;

    qDebug("Closing ticker info");

    if(reply)
    {
        reply->blockSignals(true);
        reply->abort();
    }
}

void TickerInformationToolTipLabel::hideTip()
{
    if (!hideTimer.isActive())
        hideTimer.start(300, this);
}

void TickerInformationToolTipLabel::hideTipImmediately()
{
    close(); // to trigger QEvent::Close which stops the animation
    deleteLater();
}

void TickerInformationToolTipLabel::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == hideTimer.timerId()
        || e->timerId() == expireTimer.timerId()){
        hideTimer.stop();
        expireTimer.stop();
        hideTipImmediately();
    }
}

bool TickerInformationToolTipLabel::eventFilter(QObject *o, QEvent *e)
{
    switch (e->type())
    {
        case QEvent::Leave:
            hideTip();
        break;

        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        case QEvent::Wheel:
        case QEvent::KeyPress:
            hideTipImmediately();
        break;

        default:
        break;
    }

    return false;
}

int TickerInformationToolTipLabel::getTipScreen(const QPoint &pos)
{
    if (QApplication::desktop()->isVirtualDesktop())
        return QApplication::desktop()->screenNumber(pos);
    else
        return QApplication::desktop()->screenNumber();
}

void TickerInformationToolTipLabel::placeTip(const QPoint &ps)
{
    if (testAttribute(Qt::WA_StyleSheet))
    {
        //the stylesheet need to know the real parent
        TickerInformationToolTipLabel::instance->setProperty("_q_stylesheet_parent", QVariant::fromValue(0));
        //we force the style to be the QStyleSheetStyle, and force to clear the cache as well.
        TickerInformationToolTipLabel::instance->setStyleSheet(QLatin1String("/* */"));

        // Set up for cleaning up this later...
        TickerInformationToolTipLabel::instance->styleSheetParent = 0;
    }

    QPoint pos = ps.isNull() ? lastPos : ps;
    QRect screen = QApplication::desktop()->screenGeometry(getTipScreen(pos));

    lastPos = pos;

    if(pos.x() + this->width() > screen.x() + screen.width())
        pos.rx() -= 4 + this->width();
    if(pos.y() + this->height() > screen.y() + screen.height())
        pos.ry() -= 24 + this->height();
    if(pos.y() < screen.y())
        pos.setY(screen.y());
    if(pos.x() + this->width() > screen.x() + screen.width())
        pos.setX(screen.x() + screen.width() - this->width());
    if(pos.x() < screen.x())
        pos.setX(screen.x());
    if(pos.y() + this->height() > screen.y() + screen.height())
        pos.setY(screen.y() + screen.height() - this->height());

    move(pos);
}

bool TickerInformationToolTipLabel::tipChanged(const QString &text)
{
    if(TickerInformationToolTipLabel::instance->text() != text)
        return true;

    return false;
}

void TickerInformationToolTip::showText(const QPoint &pos, const QString &text, bool ticker)
{
    if(TickerInformationToolTipLabel::instance && TickerInformationToolTipLabel::instance->isVisible())
    {
        if(text.isEmpty())
        {
            TickerInformationToolTipLabel::instance->hideTip();
            return;
        }
        else if(!TickerInformationToolTipLabel::instance->fadingOut)
        {
            if(TickerInformationToolTipLabel::instance->tipChanged(text))
            {
                TickerInformationToolTipLabel::instance->reuseTip(text, ticker);
                TickerInformationToolTipLabel::instance->placeTip(pos);
            }

            return;
        }
    }

    if(!text.isEmpty())
    {
        new TickerInformationToolTipLabel(text, ticker, QApplication::desktop()->screen(TickerInformationToolTipLabel::getTipScreen(pos)));

        TickerInformationToolTipLabel::instance->placeTip(pos);
        TickerInformationToolTipLabel::instance->setObjectName(QLatin1String("qtooltip_label"));
        TickerInformationToolTipLabel::instance->show();
    }
}

Q_GLOBAL_STATIC(QPalette, tooltip_palette)

QPalette TickerInformationToolTip::palette()
{
    return *tooltip_palette();
}

void TickerInformationToolTip::setPalette(const QPalette &palette)
{
    *tooltip_palette() = palette;

    if (TickerInformationToolTipLabel::instance)
        TickerInformationToolTipLabel::instance->setPalette(palette);
}

#include "tickerinformationtooltip.moc"
