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

#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileInfo>
#include <QObject>
#include <QFile>
#include <QDir>

#include <cstring>

#include <windows.h>

#include "settings.h"
#include "tools.h"

// serialize/deserialize LinkPoint
static QDataStream &operator<<(QDataStream &out, const LinkPoint &lp)
{
    out << lp.name << lp.points;
    return out;
}

static QDataStream &operator>>(QDataStream &in, LinkPoint &lp)
{
    in >> lp.name >> lp.points;
    return in;
}

// serialize/deserialize Qt::AlignmentFlag
static QDataStream &operator<<(QDataStream &out, const Qt::AlignmentFlag &f)
{
    QString s;

    switch(f)
    {
        case Qt::AlignRight: s = "right"; break;
        case Qt::AlignCenter: s = "center"; break;

        default:
            s = "left";
        break;
    }

    out << s;
    return out;
}

static QDataStream &operator>>(QDataStream &in, Qt::AlignmentFlag &f)
{
    QString s;
    in >> s;

    if(s == "right")
        f = Qt::AlignRight;
    else if(s == "center")
        f = Qt::AlignCenter;
    else
        f = Qt::AlignLeft;

    return in;
}

/*******************************************************/

Settings::Settings()
{
    m_settings = new QSettings(QSettings::IniFormat,
                                QSettings::UserScope,
                                QCoreApplication::organizationName(),
                                QCoreApplication::applicationName());

    m_settings->setFallbacksEnabled(false);

    qRegisterMetaTypeStreamOperators<QList<QPoint> >("QList<QPoint>");
    qRegisterMetaTypeStreamOperators<LinkPoint>("LinkPoint");
    qRegisterMetaTypeStreamOperators<QList<LinkPoint> >("QList<LinkPoint>");
    qRegisterMetaTypeStreamOperators<Qt::AlignmentFlag>("Qt::AlignmentFlag");

    m_databaseTimestampFormat = "yyyy-MM-dd hh:mm:ss.zzz";

    m_rxTicker = QRegExp("[a-zA-Z\\-\\.$]{1,7}");

    // migrate from old settings
    if(m_settings->childGroups().isEmpty())
    {
        qDebug("Trying settings from 0.7.0");

        QStringList oldkeys;
        QSettings *old;

        // 0.7.0
        old = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Noname", "THT");

        if(!old)
            qDebug("Cannot allocate memory");
        else
        {
            old->setFallbacksEnabled(false);
            oldkeys = old->allKeys();
        }

        if(oldkeys.isEmpty())
        {
            delete old;

            qDebug("Trying settings from 0.6.0");

            // 0.6.0
            old = new QSettings(QSettings::NativeFormat, QSettings::UserScope, "Noname", "THT");

            if(!old)
                qDebug("Cannot allocate memory");
            else
            {
                old->setFallbacksEnabled(false);
                oldkeys = old->allKeys();
            }
        }
        else
        {
            // clear useless 0.6.0 settings
            QSettings old060(QSettings::NativeFormat, QSettings::UserScope, "Noname", "THT");
            old060.clear();
        }

        if(old && !oldkeys.isEmpty())
        {
            qDebug("Copying settings from the old configuration");

            foreach(QString key, oldkeys)
            {
                m_settings->setValue(key, old->value(key));
            }

            // remove old settings
            old->clear();
        }

        delete old;
    }

    // save version for future changes
    m_settings->setValue("version", NVER_STRING);
    m_settings->sync();

    memset(&m_windowsVersion, 0, sizeof(OSVERSIONINFO));
    m_windowsVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if(!GetVersionEx(&m_windowsVersion))
    {
        qDebug("Cannot get system version (%ld), falling back to XP", GetLastError());

        // fallback to XP
        m_windowsVersion.dwMajorVersion = 5;
        m_windowsVersion.dwMinorVersion = 1;
        m_windowsVersion.dwPlatformId = VER_PLATFORM_WIN32_NT;
    }
    else
        qDebug("Windows version %ld.%ld", m_windowsVersion.dwMajorVersion, m_windowsVersion.dwMinorVersion);

    // databases
    m_persistentDatabaseName = "persistent";
    m_persistentDatabasePath = QCoreApplication::applicationDirPath()
                                        + QDir::separator() + "tickers.sqlite";

    m_mutableDatabaseName = "mutable";
    QString mutablePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    m_mutableDatabasePath = mutablePath + QDir::separator() + "tickers.sqlite";

    if(!QDir().mkpath(mutablePath))
        qDebug("Cannot create a directory for mutable database");

    // default values
    m_defaultValues.insert(SETTING_SCREENSHOT_TEXT_COLOR, Qt::black);
    m_defaultValues.insert(SETTING_ELLIPSE_FILL_COLOR, QColor(0, 255, 0, 50));
    m_defaultValues.insert(SETTING_SCREENSHOT_TEXT_ALIGNMENT, Qt::AlignLeft);
    m_defaultValues.insert(SETTING_SCREENSHOT_TEXT_SIZE, -1);
    m_defaultValues.insert(SETTING_RESTORE_NEIGHBORS_AT_STARTUP, true);
    m_defaultValues.insert(SETTING_MINI_TICKER_ENTRY, true);
    m_defaultValues.insert(SETTING_ALLOW_DUPLICATES, true);
    m_defaultValues.insert(SETTING_SAVE_GEOMETRY, true);
}

Settings::~Settings()
{
    delete m_settings;
}

bool Settings::preloadMode() const
{
    static bool isPreload = (QCoreApplication::arguments().indexOf("--preload") >= 0);

    return isPreload;
}

void Settings::sync()
{
    m_settings->sync();
}

void Settings::setCheckBoxState(const QString &checkbox, bool checked, SyncType sync)
{
    m_settings->setValue("settings/checkbox-" + checkbox, (int)checked);

    if(sync == Sync)
        m_settings->sync();
}

int Settings::checkBoxState(const QString &checkbox)
{
    return m_settings->value("settings/checkbox-" + checkbox, -1).toInt();
}

void Settings::setNumberOfLists(int n, SyncType sync)
{
    m_settings->setValue("settings/number-of-lists", n);

    if(sync == Sync)
        m_settings->sync();
}

int Settings::numberOfLists()
{
    bool ok;

    int nlists = m_settings->value("settings/number-of-lists", 3).toUInt(&ok);

    if(!ok)
        nlists = 3;

    if(nlists < 1 || nlists > maximumNumberOfLists())
        nlists = 3;

    return nlists;
}

void Settings::setTickersForGroup(int group, const QStringList &tickers, SyncType sync)
{
    m_settings->beginGroup(QString("tickers-%1").arg(group));
    m_settings->setValue("tickers", tickers);
    m_settings->endGroup();

    if(sync == Sync)
        m_settings->sync();
}

QStringList Settings::tickersForGroup(int group)
{
    m_settings->beginGroup(QString("tickers-%1").arg(group));
    QStringList tickers = m_settings->value("tickers").toStringList();
    m_settings->endGroup();

    return tickers;
}

void Settings::removeTickers(int group, SyncType sync)
{
    m_settings->beginGroup(QString("tickers-%1").arg(group));
    m_settings->remove(QString());
    m_settings->endGroup();

    if(sync == Sync)
        m_settings->sync();
}

void Settings::setHeaderForGroup(int group, const QString &header, Settings::SyncType sync)
{
    m_settings->beginGroup(QString("tickers-%1").arg(group));
    m_settings->setValue("header", header);
    m_settings->endGroup();

    if(sync == Sync)
        m_settings->sync();
}

QString Settings::headerForGroup(int group)
{
    m_settings->beginGroup(QString("tickers-%1").arg(group));
    QString header = m_settings->value("header").toString();
    m_settings->endGroup();

    return header;
}

Settings* Settings::instance()
{
    static Settings *m_inst = new Settings;

    return m_inst;
}

QDateTime Settings::readTimestamp(const QString &fileName) const
{
    QFile file(fileName + ".timestamp");

    if(file.open(QIODevice::ReadOnly))
        return QDateTime::fromString(file.readAll().trimmed(), m_databaseTimestampFormat);

    return QDateTime();
}

void Settings::fillTranslations()
{
    //: Russian language
    m_translations.insert(QObject::tr("Russian"), "ru");

    //: Ukrainian language
    m_translations.insert(QObject::tr("Ukrainian"), "uk");

    //: English language
    m_translations.insert(QObject::tr("English"), "en");
}

QPoint Settings::point(const QString &key)
{
    QString fullKey = "settings/" + key;

    if(m_settings->contains(fullKey))
        return m_settings->value(fullKey).value<QPoint>();
    else
        return Tools::invalidQPoint;
}
