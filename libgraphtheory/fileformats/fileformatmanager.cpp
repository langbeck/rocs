/*
 *  Copyright 2012-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) version 3, or any
 *  later version accepted by the membership of KDE e.V. (or its
 *  successor approved by the membership of KDE e.V.), which shall
 *  act as a proxy defined in Section 6 of version 3 of the license.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fileformatmanager.h"
#include "fileformatinterface.h"

#include <KPluginLoader>
#include <KPluginMetaData>
#include <KServiceTypeTrader>
#include <QString>
#include <QDir>
#include <QDirIterator>
#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>

using namespace GraphTheory;

Q_LOGGING_CATEGORY(FILEFORMAT, "rocs.graphtheory.fileformat")

class GraphTheory::FileFormatManagerPrivate
{
public:
    FileFormatManagerPrivate()
        : defaultGraphFilePlugin(0)
    {
        QLoggingCategory::setFilterRules(QStringLiteral("rocs.graphtheory.fileformat.debug=false"));
    }

    ~FileFormatManagerPrivate()
    { }

    QList<FileFormatInterface*> backends;
    FileFormatInterface *defaultGraphFilePlugin;
};

FileFormatManager::FileFormatManager()
    : d(new FileFormatManagerPrivate)
{
    loadBackends();
}

FileFormatManager::~FileFormatManager()
{

}

QList<FileFormatInterface*> FileFormatManager::backends() const
{
    return d->backends;
}

QList<FileFormatInterface*> FileFormatManager::backends(PluginType type) const
{
    QList<FileFormatInterface*> backends;
    foreach(FileFormatInterface *backend, d->backends) {
        switch(type) {
            case Import:
                if (backend->pluginCapability() == FileFormatInterface::ImportOnly
                    || backend->pluginCapability() == FileFormatInterface::ImportAndExport)
                {
                    backends.append(backend);
                }
                break;
            case Export:
                if (backend->pluginCapability() == FileFormatInterface::ExportOnly
                    || backend->pluginCapability() == FileFormatInterface::ImportAndExport)
                {
                    backends.append(backend);
                }
                break;
            default:
                break;
        }
    }
    return backends;
}

void FileFormatManager::loadBackends()
{
    // remove all present backends
    foreach(FileFormatInterface *f, d->backends) {
        delete f;
    }
    d->backends.clear();

    // dirs to check for plugins
    QStringList dirsToCheck;
    foreach (const QString &directory, QCoreApplication::libraryPaths()) {
        dirsToCheck << directory + QDir::separator() + "rocs/fileformats";
    }

    // load plugins
    QPluginLoader loader;
    foreach (const QString &dir, dirsToCheck) {
        QVector<KPluginMetaData> metadataList = KPluginLoader::findPlugins(dir,[=](const KPluginMetaData &data){
            return data.serviceTypes().contains("rocs/graphtheory/fileformat");
        });
        for (const auto &metadata : metadataList) {
            loader.setFileName(metadata.fileName());
            qCDebug(FILEFORMAT) << "Load Plugin: " << metadata.name();
            if (!loader.load()) {
                qCCritical(FILEFORMAT) << "Error while loading plugin: " << metadata.name();
            }
            KPluginFactory *factory = KPluginLoader(loader.fileName()).factory();
            FileFormatInterface *plugin = factory->create<FileFormatInterface>(this);
            d->backends.append(plugin);
        }
    }

    // load static plugins
    d->defaultGraphFilePlugin = backendByExtension("graph2");
}

FileFormatInterface * FileFormatManager::backendByExtension(const QString &ext)
{
    QString suffix = ext.section('.', -1); // get suffix
    foreach(FileFormatInterface * p,  d->backends) {
        if (p->extensions().join(";").contains(suffix, Qt::CaseInsensitive)) {
            return p;
        }
    }
    return 0;
}

FileFormatInterface * FileFormatManager::defaultBackend()
{
    return d->defaultGraphFilePlugin;
}
