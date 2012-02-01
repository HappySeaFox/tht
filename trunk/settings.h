#ifndef SETTINGS_H
#define SETTINGS_H

#include <QStringList>
#include <QSettings>
#include <QPoint>
#include <QSize>

class Settings
{
public:
    static Settings* instance();

    enum SyncType { SyncTypeNoSync, SyncTypeSync };

    void sync();

    void setNyseOnly(bool, SyncType sync = SyncTypeSync);
    bool nyseOnly();

    void setOnTop(bool, SyncType sync = SyncTypeSync);
    bool onTop();

    void setHideToTray(bool, SyncType sync = SyncTypeSync);
    bool hideToTray();

    void setTrayNoticeSeen(bool, SyncType sync = SyncTypeSync);
    bool trayNoticeSeen();

    void setSaveGeometry(bool, SyncType sync = SyncTypeSync);
    bool saveGeometry();

    int maximumNumberOfLists() const;

    void setNumberOfLists(int, SyncType sync = SyncTypeSync);
    int numberOfLists();

    void setWindowSize(const QSize &, SyncType sync = SyncTypeSync);
    QSize windowSize();

    void setWindowPosition(const QPoint &, SyncType sync = SyncTypeSync);
    QPoint windowPosition();

    void setSaveTickers(bool, SyncType sync = SyncTypeSync);
    bool saveTickers();

    void saveTickersForGroup(int group, const QStringList &tickers, SyncType sync = SyncTypeSync);
    QStringList tickersForGroup(int group);
    void removeTickers(int group, SyncType sync = SyncTypeSync);

private:
    Settings();

    template <typename T>
    T load(const QString &key, T def = T());

    template <typename T>
    void save(const QString &key, const T &value, SyncType sync = SyncTypeSync);

private:
    QSettings m_settings;
};

inline
int Settings::maximumNumberOfLists() const
{
    return 5;
}

#endif // SETTINGS_H
