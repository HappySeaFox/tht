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
#include <QObject>
#include <QFile>
#include <QDir>

#include <windows.h>

#include "settings.h"

#define FINVIZ "http://finviz.com/screener.ashx?v=411&"

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

// serialize/deserialize FinvizUrl
static QDataStream &operator<<(QDataStream &out, const FinvizUrl &fu)
{
    out << fu.name << fu.url;
    return out;
}

static QDataStream &operator>>(QDataStream &in, FinvizUrl &fu)
{
    in >> fu.name >> fu.url;
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

    qRegisterMetaTypeStreamOperators<FinvizUrl>("FinvizUrl");
    qRegisterMetaTypeStreamOperators<QList<FinvizUrl> >("QList<FinvizUrl>");

    m_databaseTimestampFormat = "yyyy-MM-dd hh:mm:ss.zzz";

    m_rxTicker = QRegExp("\\$?[a-zA-Z\\-\\.]{1,5}");

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

        // save version for future changes
        m_settings->setValue("version", NVER_STRING);
        m_settings->sync();
    }

    ZeroMemory(&m_windowsVersion, sizeof(OSVERSIONINFO));
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

    // default Finviz urls
    if(!m_settings->contains("settings/finviz-urls"))
    {
        setFinvizUrls(QList<FinvizUrl>()
                      << FinvizUrl("NYSE >1$ >300k",            QUrl(FINVIZ "f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=ticker"))
                      << FinvizUrl("NYSE >1$ >300k New High",   QUrl(FINVIZ "s=ta_newhigh&f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=-change"))
                      << FinvizUrl("NYSE >1$ >300k New Low",    QUrl(FINVIZ "s=ta_newlow&f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=-change"))
                      << FinvizUrl("NYSE >1$ >300k Volume>1.5", QUrl(FINVIZ "f=exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1,sh_relvol_o1.5&o=-change"))
                      << FinvizUrl("NYSE >1$ >300k Earn Yesterday After Close", QUrl(FINVIZ "f=earningsdate_yesterdayafter,exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=-change"))
                      << FinvizUrl("NYSE >1$ >300k Earn Today Before Open",     QUrl(FINVIZ "f=earningsdate_todaybefore,exch_nyse,geo_usa,ind_stocksonly,sh_avgvol_o300,sh_price_o1&o=-change"))
                      );
    }
}

Settings::~Settings()
{
    m_settings->sync();
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
    save<int>("checkbox-" + checkbox, checked, sync);
}

int Settings::checkBoxState(const QString &checkbox)
{
    return load<int>("checkbox-" + checkbox, -1);
}

void Settings::setTranslation(QString t, SyncType sync)
{
    save<QString>("translation", t, sync);
}

QString Settings::translation()
{
    return load<QString>("translation");
}

void Settings::setLastTickerInput(QString t, SyncType sync)
{
    save<QString>("last-ticker-input", t, sync);
}

QString Settings::lastTickerInput()
{
    return load<QString>("last-ticker-input");
}

void Settings::setLastTickerDirectory(QString dir, SyncType sync)
{
    save<QString>("last-ticker-directory", dir, sync);
}

QString Settings::lastTickerDirectory()
{
    return load<QString>("last-ticker-directory");
}

void Settings::setLastScreenShotDirectory(QString dir, SyncType sync)
{
    save<QString>("last-screenshot-directory", dir, sync);
}

QString Settings::lastScreenShotDirectory()
{
    return load<QString>("last-screenshot-directory");
}

void Settings::setRestoreNeighborsAtStartup(bool s, SyncType sync)
{
    save<bool>("restore-neighbors-at-startup", s, sync);
}

bool Settings::restoreNeighborsAtStartup()
{
    return load<bool>("restore-neighbors-at-startup", true);
}

void Settings::setShowNeighborsAtStartup(bool s, SyncType sync)
{
    save<bool>("neighbors-at-startup", s, sync);
}

bool Settings::showNeighborsAtStartup()
{
    return load<bool>("neighbors-at-startup", false);
}

void Settings::setMiniTickerEntry(bool mte, SyncType sync)
{
    save<bool>("mini-ticker-entry", mte, sync);
}

bool Settings::miniTickerEntry()
{
    return load<bool>("mini-ticker-entry", true);
}

void Settings::setLinks(const QList<LinkPoint> &links, SyncType sync)
{
    save<QList<LinkPoint> >("links", links, sync);
}

QList<LinkPoint> Settings::links()
{
    return load<QList<LinkPoint> >("links");
}

void Settings::setFinvizUrls(const QList<FinvizUrl> &fu, SyncType sync)
{
    save<QList<FinvizUrl> >("finviz-urls", fu, sync);
}

QList<FinvizUrl> Settings::finvizUrls()
{
    return load<QList<FinvizUrl> >("finviz-urls");
}

void Settings::setAllowDuplicates(bool allow, SyncType sync)
{
    save<bool>("allow-duplicates", allow, sync);
}

bool Settings::allowDuplicates()
{
    return load<bool>("allow-duplicates", true);
}

void Settings::setNyseOnly(bool n, SyncType sync)
{
    save<bool>("nyse-only", n, sync);
}

bool Settings::nyseOnly()
{
    return load<bool>("nyse-only", false);
}

void Settings::setOnTop(bool ontop, SyncType sync)
{
    save<bool>("ontop", ontop, sync);
}

bool Settings::onTop()
{
    return load<bool>("ontop", false);
}

void Settings::setHideToTray(bool hide, SyncType sync)
{
    save<bool>("tray", hide, sync);
}

bool Settings::hideToTray()
{
    return load<bool>("tray", false);
}

void Settings::setTrayNoticeSeen(bool seen, SyncType sync)
{
    save<bool>("tray-notice-seen", seen, sync);
}

bool Settings::trayNoticeSeen()
{
    return load<bool>("tray-notice-seen", false);
}

void Settings::setSaveGeometry(bool s, SyncType sync)
{
    save<bool>("save-geometry", s, sync);
}

bool Settings::saveGeometry()
{
    return load<bool>("save-geometry", true);
}

void Settings::setNumberOfLists(int n, SyncType sync)
{
    save<int>("number-of-lists", n, sync);
}

int Settings::numberOfLists()
{
    bool ok;

    m_settings->beginGroup("settings");
    int nlists = m_settings->value("number-of-lists", 3).toUInt(&ok);
    m_settings->endGroup();

    if(!ok)
        nlists = 3;

    if(nlists < 1 || nlists > maximumNumberOfLists())
        nlists = 3;

    return nlists;
}

void Settings::setWindowSize(const QSize &s, SyncType sync)
{
    save<QSize>("size", s, sync);
}

QSize Settings::windowSize()
{
    return load<QSize>("size");
}

void Settings::setWindowPosition(const QPoint &p, SyncType sync)
{
    save<QPoint>("position", p, sync);
}

QPoint Settings::windowPosition()
{
    return load<QPoint>("position");
}

void Settings::setNeighborsWindowSize(const QSize &s, SyncType sync)
{
    save<QSize>("neighbors-size", s, sync);
}

QSize Settings::neighborsWindowSize()
{
    return load<QSize>("neighbors-size");
}

void Settings::setNeighborsWindowPosition(const QPoint &p, SyncType sync)
{
    save<QPoint>("neighbors-position", p, sync);
}

QPoint Settings::neighborsWindowPosition()
{
    return load<QPoint>("neighbors-position");
}

void Settings::setSaveTickers(bool s, SyncType sync)
{
    save<bool>("save-tickers", s, sync);
}

bool Settings::saveTickers()
{
    return load<bool>("save-tickers", false);
}

void Settings::saveTickersForGroup(int group, const QStringList &tickers, SyncType sync)
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

Settings* Settings::instance()
{
    static Settings *m_inst = new Settings;

    return m_inst;
}

template <typename T>
T Settings::load(const QString &key, const T &def)
{
    m_settings->beginGroup("settings");
    QVariant value = m_settings->value(key, QVariant::fromValue(def));
    m_settings->endGroup();

    return value.value<T>();
}

template <typename T>
void Settings::save(const QString &key, const T &value, SyncType sync)
{
    m_settings->beginGroup("settings");
    m_settings->setValue(key, QVariant::fromValue(value));
    m_settings->endGroup();

    if(sync == Sync)
        m_settings->sync();
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
    m_translations.insert("ru", QObject::tr("Russian"));
    m_translations.insert("uk", QObject::tr("Ukrainian"));
    m_translations.insert("en_US", QObject::tr("English"));
}
