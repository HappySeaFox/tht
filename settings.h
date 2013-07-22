/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QStringList>
#include <QDateTime>
#include <QSettings>
#include <QVariant>
#include <QString>
#include <QPoint>
#include <QColor>
#include <QSize>
#include <QList>
#include <QHash>
#include <QMap>

#include <windows.h>

struct LinkedWindow
{
    LinkedWindow(bool _master = false, const QPoint &_point = QPoint());

    bool master;
    QPoint point;
};

struct LinkPointSession
{
    LinkPointSession(const QString &_name = QString(), const QList<LinkedWindow> &_windows = QList<LinkedWindow>());

    QString name;
    QList<LinkedWindow> windows;
};

Q_DECLARE_METATYPE(QList<QPoint>)
Q_DECLARE_METATYPE(LinkedWindow)
Q_DECLARE_METATYPE(QList<LinkedWindow>)
Q_DECLARE_METATYPE(LinkPointSession)
Q_DECLARE_METATYPE(QList<LinkPointSession>)
Q_DECLARE_METATYPE(Qt::AlignmentFlag)

#define SETTINGS_GET_BOOL Settings::instance()->value<bool>
#define SETTINGS_SET_BOOL Settings::instance()->setValue<bool>

#define SETTINGS_GET_INT Settings::instance()->value<int>
#define SETTINGS_SET_INT Settings::instance()->setValue<int>

#define SETTINGS_GET_LONG Settings::instance()->value<long>
#define SETTINGS_SET_LONG Settings::instance()->setValue<long>

#define SETTINGS_GET_DOUBLE Settings::instance()->value<double>
#define SETTINGS_SET_DOUBLE Settings::instance()->setValue<double>

#define SETTINGS_GET_STRING Settings::instance()->value<QString>
#define SETTINGS_SET_STRING Settings::instance()->setValue<QString>

#define SETTINGS_GET_COLOR Settings::instance()->value<QColor>
#define SETTINGS_SET_COLOR Settings::instance()->setValue<QColor>

#define SETTINGS_GET_ALIGNMENT Settings::instance()->value<Qt::AlignmentFlag>
#define SETTINGS_SET_ALIGNMENT Settings::instance()->setValue<Qt::AlignmentFlag>

#define SETTINGS_GET_POINTS Settings::instance()->value<QList<QPoint> >
#define SETTINGS_SET_POINTS Settings::instance()->setValue<QList<QPoint> >

#define SETTINGS_GET_LINKED_WINDOWS Settings::instance()->value<QList<LinkedWindow> >
#define SETTINGS_SET_LINKED_WINDOWS Settings::instance()->setValue<QList<LinkedWindow> >

#define SETTINGS_GET_LINKS Settings::instance()->value<QList<LinkPointSession> >
#define SETTINGS_SET_LINKS Settings::instance()->setValue<QList<LinkPointSession> >

#define SETTINGS_GET_SIZE Settings::instance()->value<QSize>
#define SETTINGS_SET_SIZE Settings::instance()->setValue<QSize>

#define SETTINGS_GET_POINT Settings::instance()->value<QPoint>
#define SETTINGS_SET_POINT Settings::instance()->setValue<QPoint>

#define SETTING_RESTORE_LINKS_AT_STARTUP     "restore-links-at-startup"
#define SETTING_LAST_LINKS                   "last-links"
#define SETTING_SHOW_COMMENTS                "show-comments"
#define SETTING_LIST_HEADER                  "list-header"
#define SETTING_LIST_BUTTONS                 "list-buttons"
#define SETTING_FOOLSDAY_SEEN                "foolsday-seen"
#define SETTING_ELLIPSE_FILL_COLOR           "ellipse-fill-color"
#define SETTING_SCREENSHOT_TEXT_COLOR        "screenshot-text-color"
#define SETTING_SCREENSHOT_TEXT_ALIGNMENT    "screenshot-text-alignment"
#define SETTING_SCREENSHOT_TEXT_SIZE         "screenshot-text-size"
#define SETTING_SCREENSHOT_TEXT              "screenshot-text"
#define SETTING_TRANSLATION                  "translation"
#define SETTING_LAST_TICKER_INPUT            "last-ticker-input"
#define SETTING_LAST_TICKER_DIRECTORY        "last-ticker-directory"
#define SETTING_LAST_SCREENSHOT_DIRECTORY    "last-screenshot-directory"
#define SETTING_RESTORE_NEIGHBORS_AT_STARTUP "restore-neighbors-at-startup"
#define SETTING_SHOW_NEIGHBORS_AT_STARTUP    "neighbors-at-startup"
#define SETTING_LINKS                        "links"
#define SETTING_MINI_TICKER_ENTRY            "mini-ticker-entry"
#define SETTING_ALLOW_DUPLICATES             "allow-duplicates"
#define SETTING_NYSE_ONLY                    "nyse-only"
#define SETTING_ONTOP                        "ontop"
#define SETTING_HIDE_TO_TRAY                 "tray"
#define SETTING_TRAY_NOTICE_SEEN             "tray-notice-seen"
#define SETTING_SAVE_GEOMETRY                "save-geometry"
#define SETTING_SIZE                         "size"
#define SETTING_POSITION                     "position"
#define SETTING_NEIGHBORS_SIZE               "neighbors-size"
#define SETTING_NEIGHBORS_POSITION           "neighbors-position"
#define SETTING_SAVE_TICKERS                 "save-tickers"
#define SETTING_GLOBAL_HOTKEY_SCREENSHOT     "global-hotkey-screenshot"
#define SETTING_GLOBAL_HOTKEY_RESTORE        "global-hotkey-restore"
#define SETTING_IS_WINDOW_SQUEEZED           "is-window-squeezed"
#define SETTING_DROPBOX_TOKEN                "dropbox-token"
#define SETTING_DROPBOX_TOKEN_SECRET         "dropbox-token-secret"
#define SETTING_DROPBOX_ACCESS_TOKEN         "dropbox-access-token"
#define SETTING_DROPBOX_ACCESS_TOKEN_SECRET  "dropbox-access-token-secret"

class SettingsPrivate;

/*
 *  Class to query the application settings. You can use
 *  the macroses above to query the appropriate types
 *  of values like that:
 *
 *      qDebug() << SETTINGS_GET_BOOL(SETTING_RESTORE_NEIGHBORS_AT_STARTUP);
 */
class Settings
{
public:
    static Settings* instance();

    ~Settings();

    enum SyncType { NoSync, Sync };

    /*
     *  Get the key value. If the value is not found, then
     *  the appropriate value from the known default values
     *  is returned. If the default value for the key is unknown,
     *  the C++ default value is returned ('0' for 'int', 'false' for 'bool' etc.)
     */
    template <typename T>
    T value(const QString &key);

    /*
     *  Get the key value. Return 'def' if the key is not found
     */
    template <typename T>
    T value(const QString &key, const T &def);

    /*
     *  Set the value of the key. If 'sync' is 'Sync', then call sync()
     */
    template <typename T>
    void setValue(const QString &key, const T &value, SyncType sync = Sync);

    /*
     *  Add your default values for your settings
     */
    void addDefaultValues(const QHash<QString, QVariant> &defaultValues);

    /*
     *  Returns 'true' if there exists a setting 'key'
     */
    bool contains(const QString &key) const;

    /*
     *  Sync with the storage
     */
    void sync();

    /*
     *  Available translations, hardcoded
     */
    QMap<QString, QString> translations();

    /*
     *  Current Windows version
     */
    OSVERSIONINFO windowsVersion() const;

    void rereadTimestamps();

    QString databaseTimestampFormat() const;

    QDateTime persistentDatabaseTimestamp() const;
    QDateTime mutableDatabaseTimestamp() const;

    QString persistentDatabaseName() const;
    QString persistentDatabasePath() const;

    QString mutableDatabaseName() const;
    QString mutableDatabasePath() const;

    /*
     *  Regexp to validate a ticker name
     */
    QRegExp tickerValidator() const;

    void setCheckBoxState(const QString &checkbox, bool checked, SyncType sync = Sync);
    int checkBoxState(const QString &checkbox);

    int maximumNumberOfLists() const;
    void setNumberOfLists(int, SyncType sync = Sync);
    int numberOfLists();

    void setTickersForGroup(int group, const QStringList &tickers, SyncType sync = Sync);
    QStringList tickersForGroup(int group);
    void removeTickers(int group, SyncType sync = Sync);

    void setHeaderForGroup(int group, const QString &header, SyncType sync = Sync);
    QString headerForGroup(int group);

private:
    Settings();

    QDateTime readTimestamp(const QString &fileName) const;

    void fillTranslations();

    QPoint point(const QString &key);

    QHash<QString, QVariant> &defaultValues();

    QSettings *settings();

private:
    SettingsPrivate *d;
};

/**********************************/

template <typename T>
T Settings::value(const QString &key)
{
    T def = T();
    QHash<QString, QVariant>::iterator it = defaultValues().find(key);

    if(it != defaultValues().end())
        def = it.value().value<T>();

    return value<T>(key, def);
}

template <typename T>
T Settings::value(const QString &key, const T &def)
{
    QSettings *s = settings();

    s->beginGroup("settings");
    QVariant value = s->value(key, QVariant::fromValue(def));
    s->endGroup();

    return value.value<T>();
}

template <typename T>
void Settings::setValue(const QString &key, const T &value, Settings::SyncType sync)
{
    QSettings *s = settings();

    s->beginGroup("settings");
    s->setValue(key, QVariant::fromValue(value));
    s->endGroup();

    if(sync == Sync)
        s->sync();
}

#endif // SETTINGS_H
