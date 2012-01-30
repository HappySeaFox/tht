#include "settings.h"

Settings::Settings()
{
}

void Settings::setSaveGeometry(bool save)
{
    m_settings.beginGroup("settings");
    m_settings.setValue("save-geometry", save);
    m_settings.endGroup();
    m_settings.sync();
}

bool Settings::saveGeometry()
{
    m_settings.beginGroup("settings");
    bool save = m_settings.value("save-geometry", true).toBool();
    m_settings.endGroup();

    return save;
}

void Settings::setNumberOfLists(int n)
{
    m_settings.beginGroup("settings");
    m_settings.setValue("number-of-lists", n);
    m_settings.endGroup();
    m_settings.sync();
}

int Settings::numberOfLists()
{
    bool ok;

    m_settings.beginGroup("settings");
    int nlists = m_settings.value("number-of-lists", 3).toUInt(&ok);
    m_settings.endGroup();

    if(!ok)
        nlists = 3;

    if(nlists < 1 || nlists > 5)
        nlists = 3;

    return nlists;
}

void Settings::setWindowSize(const QSize &s)
{
    m_settings.beginGroup("settings");
    m_settings.setValue("size", s);
    m_settings.endGroup();
    m_settings.sync();
}

QSize Settings::windowSize()
{
    m_settings.beginGroup("settings");
    QSize sz = m_settings.value("size").toSize();
    m_settings.endGroup();

    return sz;
}

void Settings::setWindowPosition(const QPoint &p)
{
    m_settings.beginGroup("settings");
    m_settings.setValue("position", p);
    m_settings.endGroup();
    m_settings.sync();
}

QPoint Settings::windowPosition()
{
    m_settings.beginGroup("settings");
    QPoint p = m_settings.value("position").toPoint();
    m_settings.endGroup();

    return p;
}

void Settings::setSaveTickers(bool save)
{
    m_settings.beginGroup("settings");
    m_settings.setValue("save-tickers", save);
    m_settings.endGroup();
    m_settings.sync();
}

bool Settings::saveTickers()
{
    m_settings.beginGroup("settings");
    bool save = m_settings.value("save-tickers", true).toBool();
    m_settings.endGroup();

    return save;
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
