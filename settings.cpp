#include "settings.h"

Settings::Settings()
{
}

void Settings::setOnTop(bool ontop)
{
    save<bool>("ontop", ontop);
}

bool Settings::onTop()
{
    return load<bool>("ontop", false);
}

void Settings::setHideToTray(bool hide)
{
    save<bool>("tray", hide);
}

bool Settings::hideToTray()
{
    return load<bool>("tray", false);
}

void Settings::setTrayNoticeSeen(bool seen)
{
    save<bool>("tray-notice-seen", seen);
}

bool Settings::trayNoticeSeen()
{
    return load<bool>("tray-notice-seen", false);
}

void Settings::setSaveGeometry(bool s)
{
    save<bool>("save-geometry", s);
}

bool Settings::saveGeometry()
{
    return load<bool>("save-geometry", true);
}

void Settings::setNumberOfLists(int n)
{
    save<int>("number-of-lists", n);
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

void Settings::setWindowSize(const QSize &s)
{
    save<QSize>("size", s);
}

QSize Settings::windowSize()
{
    return load<QSize>("size");
}

void Settings::setWindowPosition(const QPoint &p)
{
    save<QPoint>("position", p);
}

QPoint Settings::windowPosition()
{
    return load<QPoint>("position");
}

void Settings::setSaveTickers(bool s)
{
    save<bool>("save-tickers", s);
}

bool Settings::saveTickers()
{
    return load<bool>("save-tickers", true);
}

void Settings::saveTickersForGroup(int group, const QStringList &tickers)
{
    m_settings.beginGroup(QString("tickers-%1").arg(group));
    m_settings.setValue("tickers", tickers);
    m_settings.endGroup();
}

QStringList Settings::tickersForGroup(int group)
{
    m_settings.beginGroup(QString("tickers-%1").arg(group));
    QStringList tickers = m_settings.value("tickers").toStringList();
    m_settings.endGroup();

    return tickers;
}

void Settings::removeTickers(int group)
{
    m_settings.beginGroup(QString("tickers-%1").arg(group));
    m_settings.remove(QString());
    m_settings.endGroup();
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
void Settings::save(const QString &key, const T &value)
{
    m_settings.beginGroup("settings");
    m_settings.setValue(key, value);
    m_settings.endGroup();
    m_settings.sync();
}
