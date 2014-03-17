/*
 * This file is part of THT-lib.
 *
 * THT-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation.
 *
 * THT-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileInfo>
#include <QObject>
#include <QRegExp>
#include <QColor>
#include <QFile>
#include <QDir>
#include <QUrl>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QStandardPaths>
#endif

#include <cstring>

#include <windows.h>

#include "settings.h"
#include "tools.h"

// serialize/deserialize Qt::AlignmentFlag
static QDataStream &operator<<(QDataStream &out, const Qt::AlignmentFlag &f)
{
    out << static_cast<int>(f);
    return out;
}

static QDataStream &operator>>(QDataStream &in, Qt::AlignmentFlag &f)
{
    int fi = 0;

    in >> fi;

    f = fi ? static_cast<Qt::AlignmentFlag>(fi) : Qt::AlignLeft;

    return in;
}

/*******************************************************/

class SettingsPrivate
{
public:
    QSettings *settings;
    QString persistentDatabaseName;
    QString persistentDatabasePath;
    QString mutableDatabaseName;
    QString mutableDatabasePath;
    QDateTime persistentDatabaseTimestamp;
    QDateTime mutableDatabaseTimestamp;
    QString databaseTimestampFormat;
    QMap<QString, QString> translations;
    QHash<QString, QVariant> defaultValues;
};

/*******************************************************/

Settings::Settings()
{
    d = new SettingsPrivate;

    d->settings = new QSettings(QSettings::IniFormat,
                                QSettings::UserScope,
                                QCoreApplication::organizationName(),
                                QCoreApplication::applicationName());

    d->settings->setFallbacksEnabled(false);

    qRegisterMetaTypeStreamOperators<QList<QPoint> >("QList<QPoint>");
    qRegisterMetaTypeStreamOperators<Qt::AlignmentFlag>("Qt::AlignmentFlag");

    // save version for future changes
    d->settings->setValue("version", NVER_STRING);

    d->databaseTimestampFormat = "yyyy-MM-dd hh:mm:ss.zzz";

    // databases
    d->persistentDatabaseName = "persistent";
    d->persistentDatabasePath = QCoreApplication::applicationDirPath()
                                        + QDir::separator() + "tickers.sqlite";

    d->mutableDatabaseName = "mutable";

    QString mutablePath =
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

    d->mutableDatabasePath = mutablePath + QDir::separator() + "tickers.sqlite";

    if(!QDir().mkpath(mutablePath))
        qWarning("Cannot create a directory for mutable database");

    // default values
    d->defaultValues.insert(SETTING_RESTORE_NEIGHBORS_AT_STARTUP, true);
    d->defaultValues.insert(SETTING_LIST_BUTTONS, true);
    d->defaultValues.insert(SETTING_LIST_HEADER, true);
    d->defaultValues.insert(SETTING_MINI_TICKER_ENTRY, true);
    d->defaultValues.insert(SETTING_SAVE_GEOMETRY, true);
    d->defaultValues.insert(SETTING_GLOBAL_HOTKEY_SCREENSHOT, true);
    d->defaultValues.insert(SETTING_GLOBAL_HOTKEY_RESTORE, true);
    d->defaultValues.insert(SETTING_POSITION, Tools::invalidQPoint);
}

Settings::~Settings()
{
    delete d->settings;
    delete d;
}

void Settings::addDefaultValues(const QHash<QString, QVariant> &defaultValues)
{
    QHash<QString, QVariant>::const_iterator itEnd = defaultValues.end();

    for(QHash<QString, QVariant>::const_iterator it = defaultValues.begin();it != itEnd;++it)
    {
        d->defaultValues.insert(it.key(), it.value());
    }
}

QVariant Settings::defaultValue(const QString &key) const
{
    return d->defaultValues.value(key);
}

void Settings::sync()
{
    d->settings->sync();
}

void Settings::setCheckBoxState(const QString &checkbox, bool checked, SyncType sync)
{
    d->settings->setValue("settings/checkbox-" + checkbox, (int)checked);

    if(sync == Sync)
        d->settings->sync();
}

int Settings::checkBoxState(const QString &checkbox)
{
    return d->settings->value("settings/checkbox-" + checkbox, -1).toInt();
}

void Settings::setNumberOfLists(int n, SyncType sync)
{
    d->settings->setValue("settings/number-of-lists", n);

    if(sync == Sync)
        d->settings->sync();
}

int Settings::numberOfLists()
{
    const int defaultNumber = 3;

    bool ok;
    int nlists = d->settings->value("settings/number-of-lists", defaultNumber).toUInt(&ok);

    if(!ok || nlists < 1 || nlists > maximumNumberOfLists())
        nlists = defaultNumber;

    return nlists;
}

void Settings::setTickersForGroup(int group, const QStringList &tickers, SyncType sync)
{
    d->settings->beginGroup(QString("tickers-%1").arg(group));
    d->settings->setValue("tickers", tickers);
    d->settings->endGroup();

    if(sync == Sync)
        d->settings->sync();
}

QStringList Settings::tickersForGroup(int group)
{
    d->settings->beginGroup(QString("tickers-%1").arg(group));
    QStringList tickers = d->settings->value("tickers").toStringList();
    d->settings->endGroup();

    return tickers;
}

void Settings::removeTickers(int group, SyncType sync)
{
    d->settings->beginGroup(QString("tickers-%1").arg(group));
    d->settings->remove(QString());
    d->settings->endGroup();

    if(sync == Sync)
        d->settings->sync();
}

void Settings::setHeaderForGroup(int group, const QString &header, Settings::SyncType sync)
{
    d->settings->beginGroup(QString("tickers-%1").arg(group));
    d->settings->setValue("header", header);
    d->settings->endGroup();

    if(sync == Sync)
        d->settings->sync();
}

QString Settings::headerForGroup(int group)
{
    d->settings->beginGroup(QString("tickers-%1").arg(group));
    QString header = d->settings->value("header").toString();
    d->settings->endGroup();

    return header;
}

Settings* Settings::instance()
{
    static Settings *inst = new Settings;

    return inst;
}

QDateTime Settings::readTimestamp(const QString &fileName) const
{
    QFile file(fileName + ".timestamp");

    if(file.open(QIODevice::ReadOnly))
        return QDateTime::fromString(file.readAll().trimmed(), d->databaseTimestampFormat);

    return QDateTime();
}

void Settings::fillTranslations()
{
    d->translations.insert("en", "English");

    // http://www.loc.gov/standards/iso639-2/php/code_list.php
    d->translations.insert("da",    QString::fromUtf8("Dansk"));
    d->translations.insert("de",    QString::fromUtf8("Deutsch"));
    d->translations.insert("es",    QString::fromUtf8("Español"));
    //d->translations.insert("es_MX", QString::fromUtf8("Español Mexicano"));
    //d->translations.insert("fi", QString::fromUtf8("Suomi"));
    d->translations.insert("fr",    QString::fromUtf8("Français"));
    //d->translations.insert("he", QString::fromUtf8("עברית"));
    d->translations.insert("it",    QString::fromUtf8("Italiano"));
    d->translations.insert("ja",    QString::fromUtf8("日本語"));
    d->translations.insert("ko",    QString::fromUtf8("조선말"));
    d->translations.insert("nl",    QString::fromUtf8("Nederlands"));
    //d->translations.insert("no", QString::fromUtf8("Norsk"));
    d->translations.insert("pl",    QString::fromUtf8("Polski"));
    d->translations.insert("pt",    QString::fromUtf8("Português"));
    //d->translations.insert("pt_BR", QString::fromUtf8("Português Brasileiro"));
    d->translations.insert("ru",    QString::fromUtf8("Русский"));
    //d->translations.insert("sv", QString::fromUtf8("Svenska"));
    d->translations.insert("tr",    QString::fromUtf8("Türkçe"));
    d->translations.insert("uk",    QString::fromUtf8("Українська"));
    d->translations.insert("vi",    QString::fromUtf8("Tiếng Việt"));
    d->translations.insert("zh_CN", QString::fromUtf8("简体中文"));
    //d->translations.insert("zh_TW", QString::fromUtf8("繁體中文"));
}

QHash<QString, QVariant>& Settings::defaultValues()
{
    return d->defaultValues;
}

QSettings *Settings::settings()
{
    return d->settings;
}

bool Settings::contains(const QString &key) const
{
    return d->settings->contains(key.startsWith('/') ? ("settings" + key) : key);
}

void Settings::remove(const QString &key, Settings::SyncType sync)
{
    QSettings *s = settings();

    s->beginGroup("settings");
    s->remove(key);
    s->endGroup();

    if(sync == Sync)
        s->sync();
}

QMap<QString, QString> Settings::translations()
{
    if(d->translations.isEmpty())
        fillTranslations();

    return d->translations;
}

void Settings::rereadTimestamps()
{
    d->persistentDatabaseTimestamp = readTimestamp(d->persistentDatabasePath);
    d->mutableDatabaseTimestamp = readTimestamp(d->mutableDatabasePath);

    qDebug("Database P timestamp: %s", qPrintable(d->persistentDatabaseTimestamp.toString(d->databaseTimestampFormat)));
    qDebug("Database M timestamp: %s", qPrintable(d->mutableDatabaseTimestamp.toString(d->databaseTimestampFormat)));
}

QString Settings::databaseTimestampFormat() const
{
    return d->databaseTimestampFormat;
}

QDateTime Settings::persistentDatabaseTimestamp() const
{
    return d->persistentDatabaseTimestamp;
}

QDateTime Settings::mutableDatabaseTimestamp() const
{
    return d->mutableDatabaseTimestamp;
}

QString Settings::persistentDatabaseName() const
{
    return d->persistentDatabaseName;
}

QString Settings::mutableDatabaseName() const
{
    return d->mutableDatabaseName;
}

QString Settings::mutableDatabasePath() const
{
    return d->mutableDatabasePath;
}

QString Settings::persistentDatabasePath() const
{
    return d->persistentDatabasePath;
}

int Settings::maximumNumberOfLists() const
{
    return 8;
}
