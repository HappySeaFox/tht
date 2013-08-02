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

#include <QMutableListIterator>
#include <QHashIterator>
#include <QApplication>
#include <QStringList>
#include <QTranslator>
#include <QFileInfo>
#include <QLibrary>
#include <QVariant>
#include <QDebug>
#include <QHash>
#include <QDir>

#include "pluginloader.h"

PluginLoader* PluginLoader::instance()
{
    static PluginLoader *m_inst = new PluginLoader;

    return m_inst;
}

PluginLoader::PluginLoader(QObject *parent) : QObject(parent)
{
    QDir pluginsDir(QCoreApplication::applicationDirPath() + QDir::separator() + "plugins");

    QFileInfoList plugins = pluginsDir.entryInfoList(QStringList() << "*.dll",
                                                     QDir::Files | QDir::NoDotAndDotDot | QDir::Readable,
                                                     QDir::Name);

    QHash<QString, QStringList> deprecatedUuids;

    foreach(QFileInfo fi, plugins)
    {
        PluginLibrary p;

        p.library = new QLibrary(fi.absoluteFilePath());
        p.library->load();

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        p.plugin_create = (PluginLibrary::PluginCreateFunc)p.library->resolve("plugin_create");
        p.plugin_destroy = (PluginLibrary::PluginDestroyFunc)p.library->resolve("plugin_destroy");
#else
        *reinterpret_cast<void **>(&p.plugin_create) = p.library->resolve("plugin_create");
        *reinterpret_cast<void **>(&p.plugin_destroy) = p.library->resolve("plugin_destroy");
#endif

        if(!p.plugin_create || !p.plugin_destroy)
        {
            qWarning("Cannot load library (%s)", qPrintable(p.library->errorString()));
            p.library->unload();
            delete p.library;
        }
        else
        {
            p.plugin = p.plugin_create();

            if(!p.plugin)
            {
                qWarning("Plugin \"%s\" creation failed", qPrintable(fi.fileName()));
                continue;
            }

            QStringList uuids = p.plugin->property(THT_PLUGIN_PROPERTY_DEPRECATES_UUIDS).toString().split(',', QString::SkipEmptyParts);

            if(!uuids.isEmpty())
            {
                deprecatedUuids[
                                p.plugin->property(THT_PLUGIN_PROPERTY_NAME).toString()
                                + '/'
                                + p.plugin->property(THT_PLUGIN_PROPERTY_UUID).toString()
                                ] = uuids;
            }

            append(p);

            qDebug("Loaded plugin \"%s\"", qPrintable(fi.fileName()));
        }
    }

    QMutableListIterator<PluginLibrary> itPlugins(*this);

    while(itPlugins.hasNext())
    {
        itPlugins.next();

        QString uuid = itPlugins.value().plugin->property(THT_PLUGIN_PROPERTY_UUID).toString();

        QHashIterator<QString, QStringList> itDeprecated(deprecatedUuids);

        while(itDeprecated.hasNext())
        {
            itDeprecated.next();

            if(itDeprecated.value().indexOf(uuid) >= 0)
            {
                qDebug("Plugin \"%s/%s\" is deprecated by \"%s\"",
                       qPrintable(itPlugins.value().plugin->property(THT_PLUGIN_PROPERTY_NAME).toString()),
                       qPrintable(uuid),
                       qPrintable(itDeprecated.key()));

                itPlugins.value().plugin_destroy(itPlugins.value().plugin);
                delete itPlugins.value().library;
                itPlugins.remove();
            }
        }
    }
}

PluginLoader::~PluginLoader()
{
    iterator itEnd = end();

    for(iterator it = begin();it != itEnd;++it)
    {
        qDebug("Removing plugin \"%s\"", qPrintable((*it).library->fileName()));

        delete (*it).translator;
        (*it).plugin_destroy((*it).plugin);
        delete (*it).library;
    }

    clear();
}

void PluginLoader::init()
{
    QString ts = qApp->property("tht-translation").toString();
    QString tsDir = QCoreApplication::applicationDirPath() + QDir::separator() + "translations";

    iterator itEnd = end();

    for(iterator it = begin();it != itEnd;++it)
    {
        QFileInfo fi((*it).library->fileName());

        (*it).translator = new QTranslator;

        if(!(*it).translator)
        {
            qWarning("Translator for \"%s\" plugin creation failed", qPrintable(fi.fileName()));
            continue;
        }

        qDebug("Loading translation for plugin \"%s\": %s",
               qPrintable(fi.fileName()),
               (*it).translator->load(fi.baseName().replace('-', '_') + '_' + ts, tsDir) ? "ok" : "failed");

        qApp->installTranslator((*it).translator);

        (*it).plugin->setProperty(THT_PLUGIN_PROPERTY_FILENAME, fi.fileName());
        (*it).plugin->setProperty(THT_PLUGIN_PROPERTY_LICENSE_FILE, QString(fi.absolutePath()
                                                                            + QDir::separator()
                                                                            + fi.baseName()
                                                                            + "-LICENSE.txt"));
        (*it).plugin->init();
    }
}

QList<Plugin *> PluginLoader::byType(Plugin::Type type) const
{
    QList<Plugin *> list;

    const_iterator itEnd = end();

    for(const_iterator it = begin();it != itEnd;++it)
    {
        if((*it).plugin->type() == type)
            list.append((*it).plugin);
    }

    return list;
}

Plugin* PluginLoader::byUuid(const QString &uuid) const
{
    const_iterator itEnd = end();

    for(const_iterator it = begin();it != itEnd;++it)
    {
        if((*it).plugin->property(THT_PLUGIN_PROPERTY_UUID).toString() == uuid)
            return (*it).plugin;
    }

    return 0;
}
