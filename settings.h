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

    void setOnTop(bool);
    bool onTop();

    void setHideToTray(bool);
    bool hideToTray();

    void setTrayNoticeSeen(bool);
    bool trayNoticeSeen();

    void setSaveGeometry(bool);
    bool saveGeometry();

    int maximumNumberOfLists() const;

    void setNumberOfLists(int);
    int numberOfLists();

    void setWindowSize(const QSize &);
    QSize windowSize();

    void setWindowPosition(const QPoint &);
    QPoint windowPosition();

    void setSaveTickers(bool);
    bool saveTickers();

    void saveTickersForGroup(int group, const QStringList &tickers);
    QStringList tickersForGroup(int group);
    void removeTickers(int group);

private:
    Settings();

    template <typename T>
    T load(const QString &key, T def = T());

    template <typename T>
    void save(const QString &key, const T &value);

private:
    QSettings m_settings;
};

inline
int Settings::maximumNumberOfLists() const
{
    return 5;
}

#endif // SETTINGS_H
