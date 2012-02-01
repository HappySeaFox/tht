#include "settings.h"

Settings::Settings()
{
}

void Settings::sync()
{
    m_settings.sync();
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

    m_settings.beginGroup("settings");
    int nlists = m_settings.value("number-of-lists", 3).toUInt(&ok);
    m_settings.endGroup();

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
    return load<bool>("save-tickers", true);
}

void Settings::saveTickersForGroup(int group, const QStringList &tickers, SyncType sync)
{
    m_settings.beginGroup(QString("tickers-%1").arg(group));
    m_settings.setValue("tickers", tickers);
    m_settings.endGroup();

    if(sync == SyncTypeSync)
        m_settings.sync();
}

QStringList Settings::tickersForGroup(int group)
{
    m_settings.beginGroup(QString("tickers-%1").arg(group));
    QStringList tickers = m_settings.value("tickers").toStringList();
    m_settings.endGroup();

    return tickers;
}

void Settings::removeTickers(int group, SyncType sync)
{
    m_settings.beginGroup(QString("tickers-%1").arg(group));
    m_settings.remove(QString());
    m_settings.endGroup();

    if(sync == SyncTypeSync)
        m_settings.sync();
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
    m_settings.beginGroup("settings");
    T value = m_settings.value(key, QVariant(def)).value<T>();
    m_settings.endGroup();

    return value;
}

template <typename T>
void Settings::save(const QString &key, const T &value, SyncType sync)
{
    m_settings.beginGroup("settings");
    m_settings.setValue(key, value);
    m_settings.endGroup();

    if(sync == SyncTypeSync)
        m_settings.sync();
}
