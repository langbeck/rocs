/*
    This file is part of Rocs.
    Copyright (C) 2011 Andreas Cord-Landwehr <cola@uni-paderborn.de>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "AssignValuesToolsPlugin.h"
#include "AssignValuesWidget.h"
#include "../ToolsPluginInterface.h"
#include <CoreTypes.h>
#include <Document.h>
#include <DataStructure.h>
#include <DocumentManager.h>

#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QLabel>
#include <QFont>
#include <QDesktopWidget>
#include <QApplication>

#include <KAboutData>
#include <kgenericfactory.h>
#include <KLocalizedString>

#include <map>
#include "ui_AssignValuesWidget.h"

K_PLUGIN_FACTORY(ToolsPluginFactory, registerPlugin<AssignValuesToolPlugin>();)

AssignValuesToolPlugin::AssignValuesToolPlugin(QObject* parent,  const QList<QVariant> & /* args*/)
    : ToolsPluginInterface("rocs_assignvaluesplugin", parent)
{

}

AssignValuesToolPlugin::~AssignValuesToolPlugin()
{

}

void AssignValuesToolPlugin::run(Document *document) const
{
    if (document == 0) {
        document = DocumentManager::self().activeDocument();
    }
    QPointer<AssignValuesWidget> dialog = new AssignValuesWidget(document);

    // data structures to selector box
    QList< DataStructurePtr > dsList = document->dataStructures();
    QStringList dsNames;
    foreach(DataStructurePtr ds, dsList) {
        dsNames << ds->name();
    }
    dialog->addDataStructures(dsNames);

    dialog->exec();
    return;
}

#include "AssignValuesToolsPlugin.moc"
