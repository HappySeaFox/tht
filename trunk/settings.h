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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QStringList>
#include <QDateTime>
#include <QSettings>
#include <QRegExp>
#include <QPoint>
#include <QSize>

#include <windows.h>

class Settings
{
public:
    static Settings* instance();

    ~Settings();

    enum SyncType { NoSync, Sync };

    void sync();

    OSVERSIONINFO version() const;

    void setCheckBoxState(const QString &checkbox, bool checked, SyncType sync = Sync);
    int checkBoxState(const QString &checkbox);

    QString databaseTimestampFormat() const;

    QDateTime persistentDatabaseTimestamp() const;
    QDateTime mutableDatabaseTimestamp() const;

    QString persistentDatabaseName() const;
    QString persistentDatabasePath() const;

    QString mutableDatabaseName() const;
    QString mutableDatabasePath() const;

    bool preloadMode() const;

    QRegExp tickerValidator() const;

    void setLastTickerInput(QString, SyncType sync = Sync);
    QString lastTickerInput();

    void setLastTickerDirectory(QString, SyncType sync = Sync);
    QString lastTickerDirectory();

    void setLastScreenShotDirectory(QString, SyncType sync = Sync);
    QString lastScreenShotDirectory();

    void setMiniTickerEntry(bool, SyncType sync = Sync);
    bool miniTickerEntry();

    void setAllowDuplicates(bool, SyncType sync = Sync);
    bool allowDuplicates();

    void setNyseOnly(bool, SyncType sync = Sync);
    bool nyseOnly();

    void setOnTop(bool, SyncType sync = Sync);
    bool onTop();

    void setHideToTray(bool, SyncType sync = Sync);
    bool hideToTray();

    void setTrayNoticeSeen(bool, SyncType sync = Sync);
    bool trayNoticeSeen();

    void setSaveGeometry(bool, SyncType sync = Sync);
    bool saveGeometry();

    int maximumNumberOfLists() const;

    void setNumberOfLists(int, SyncType sync = Sync);
    int numberOfLists();

    void setWindowSize(const QSize &, SyncType sync = Sync);
    QSize windowSize();

    void setWindowPosition(const QPoint &, SyncType sync = Sync);
    QPoint windowPosition();

    void setSaveTickers(bool, SyncType sync = Sync);
    bool saveTickers();

    void saveTickersForGroup(int group, const QStringList &tickers, SyncType sync = Sync);
    QStringList tickersForGroup(int group);
    void removeTickers(int group, SyncType sync = Sync);

private:
    Settings();

    template <typename T>
    T load(const QString &key, const T &def = T());

    template <typename T>
    void save(const QString &key, const T &value, SyncType sync = Sync);

    QDateTime readTimestamp(const QString &fileName) const;

private:
    QSettings *m_settings;
    QRegExp m_rxTicker;
    OSVERSIONINFO m_version;
    QString m_persistentDatabaseName;
    QString m_persistentDatabasePath;
    QString m_mutableDatabaseName;
    QString m_mutableDatabasePath;
    QDateTime m_persistentDatabaseTimestamp;
    QDateTime m_mutableDatabaseTimestamp;
    QString m_databaseTimestampFormat;
};

inline
QString Settings::databaseTimestampFormat() const
{
    return m_databaseTimestampFormat;
}

inline
QDateTime Settings::persistentDatabaseTimestamp() const
{
    return m_persistentDatabaseTimestamp;
}

inline
QDateTime Settings::mutableDatabaseTimestamp() const
{
    return m_mutableDatabaseTimestamp;
}

inline
QString Settings::persistentDatabaseName() const
{
    return m_persistentDatabaseName;
}

inline
QString Settings::mutableDatabaseName() const
{
    return m_mutableDatabaseName;
}

inline
QString Settings::mutableDatabasePath() const
{
    return m_mutableDatabasePath;
}

inline
QString Settings::persistentDatabasePath() const
{
    return m_persistentDatabasePath;
}

inline
OSVERSIONINFO Settings::version() const
{
    return m_version;
}

inline
QRegExp Settings::tickerValidator() const
{
    return m_rxTicker;
}

inline
int Settings::maximumNumberOfLists() const
{
    return 5;
}

#endif // SETTINGS_H
