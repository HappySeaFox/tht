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

    bool onTop();
    void setOnTop(bool);

    void setSaveGeometry(bool);
    bool saveGeometry();

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

    QSettings m_settings;
};

#endif // SETTINGS_H
