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

#include "settings.h"

Settings::Settings()
{
    m_settings = new QSettings(QSettings::IniFormat,
                                QSettings::UserScope,
                                QCoreApplication::organizationName(),
                                QCoreApplication::applicationName());

    m_settings->setFallbacksEnabled(false);

    m_rxTicker = QRegExp("\\$?[a-zA-Z\\-]{1,5}");

    // migrate from 0.6.0
    if(m_settings->childGroups().isEmpty())
    {
        QSettings old;
        QStringList oldkeys = old.allKeys();

        if(!oldkeys.isEmpty())
        {
            foreach(QString key, oldkeys)
            {
                m_settings->setValue(key, old.value(key));
            }

            m_settings->sync();
        }
    }
}

Settings::~Settings()
{
    m_settings->sync();
    delete m_settings;
}

void Settings::sync()
{
    m_settings->sync();
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

    if(sync == SyncTypeSync)
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

    if(sync == SyncTypeSync)
        m_settings->sync();
}

Settings* Settings::instance()
{
    static Settings *m_inst = 0;

    if(!m_inst)
        m_inst = new Settings;

    return m_inst;
}

template <typename T>
T Settings::load(const QString &key, T def)
{
    m_settings->beginGroup("settings");
    T value = m_settings->value(key, QVariant(def)).value<T>();
    m_settings->endGroup();

    return value;
}

template <typename T>
void Settings::save(const QString &key, const T &value, SyncType sync)
{
    m_settings->beginGroup("settings");
    m_settings->setValue(key, value);
    m_settings->endGroup();

    if(sync == SyncTypeSync)
        m_settings->sync();
}
