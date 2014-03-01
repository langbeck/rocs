/*
    This file is part of Rocs.
    Copyright 2013  Andreas Cord-Landwehr <cola@uni-paderborn.de>

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

#include "ObjectDocumentation.h"
#include <QStringList>
#include <QDebug>

ObjectDocumentation::ObjectDocumentation(QObject* parent)
    : QObject(parent)
{
}

ObjectDocumentation::~ObjectDocumentation()
{
}

void ObjectDocumentation::setId(const QString &id)
{
    _id = id;
}

QString ObjectDocumentation::id() const
{
    return _id;
}

void ObjectDocumentation::setComponentType(const QString& compenentTypeName)
{
    if (compenentTypeName == "DataStructure") {
        _componentType = DataStructure;
        return;
    }
    if (compenentTypeName == "Data") {
        _componentType = Data;
        return;
    }
    if (compenentTypeName == "Pointer") {
        _componentType = Pointer;
        return;
    }
    if (compenentTypeName == "EngineComponent") {
        _componentType = EngineComponent;
        return;
    }

    // else
    qCritical() << "Unable to understand componentType " << compenentTypeName;
    _componentType = EngineComponent;
}

ObjectDocumentation::ComponentType ObjectDocumentation::componentType() const
{
    return _componentType;
}

void ObjectDocumentation::setTitle(const QString &title)
{
    _title = title;
}

QString ObjectDocumentation::title() const
{
    return _title;
}

void ObjectDocumentation::setDescription(const QStringList &description)
{
    _description = description;
}

QVariantList ObjectDocumentation::description() const
{
    QVariantList list;
    foreach (QString paragraph, _description) {
        list << paragraph;
    }
    return list;
}

void ObjectDocumentation::setSyntaxExample(const QString &syntaxExample)
{
    _syntaxExample = syntaxExample;
}

QString ObjectDocumentation::syntaxExample() const
{
    return _syntaxExample;
}

void ObjectDocumentation::addProperty(PropertyDocumentation* property)
{
    _properties.append(property);
}

QList< PropertyDocumentation* > ObjectDocumentation::properties() const
{
    return _properties;
}

void ObjectDocumentation::addMethod(MethodDocumentation* method)
{
    _methods.append(method);
}

QList< MethodDocumentation* > ObjectDocumentation::methods() const
{
    return _methods;
}

void ObjectDocumentation::setObjectParent(const QString& id)
{
    _objectParent = id;
}

QString ObjectDocumentation::objectParent() const
{
    return _objectParent;
}

QString ObjectDocumentation::apiDocumentIdentifier()
{
    return _id;
}
