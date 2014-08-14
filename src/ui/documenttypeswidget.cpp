/*
 *  Copyright 2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "documenttypeswidget.h"
#include "documenttypesdelegate.h"
#include "libgraphtheory/models/nodetypemodel.h"
#include "libgraphtheory/models/edgetypemodel.h"
#include "libgraphtheory/nodetype.h"
#include <KLocalizedString>
#include <KWidgetItemDelegate>
#include <QGridLayout>
#include <QListView>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

using namespace GraphTheory;

DocumentTypesWidget::DocumentTypesWidget(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);

    layout->addWidget(new QLabel(i18nc("@title", "Node Types")));

    QPushButton *createNodeTypeButton = new QPushButton(this);
    createNodeTypeButton->setText(i18n("Create Type"));
    layout->addWidget(createNodeTypeButton);

    QListView *view = new QListView(this);
    DocumentTypesDelegate *delegate = new DocumentTypesDelegate(view);
    view->setItemDelegate(delegate);
    view->setModel(&m_nodeTypeModel);
    layout->addWidget(view);

    connect(delegate, SIGNAL(colorChanged(QModelIndex,QColor)), this, SLOT(onNodeTypeColorChanged(QModelIndex,QColor)));
    connect(delegate, SIGNAL(nameChanged(QModelIndex,QString)), this, SLOT(onNodeTypeNameChanged(QModelIndex,QString)));
    connect(createNodeTypeButton, SIGNAL(clicked(bool)), this, SLOT(onCreateEdgeType()));

    setLayout(layout);
    show();
}

DocumentTypesWidget::~DocumentTypesWidget()
{

}

void DocumentTypesWidget::setDocument(GraphDocumentPtr document)
{
    m_document = document;
    m_nodeTypeModel.setDocument(document);
    m_edgeTypeModel.setDocument(document);
}

void DocumentTypesWidget::onNodeTypeColorChanged(const QModelIndex &index, const QColor &color)
{
    m_nodeTypeModel.setData(index, QVariant::fromValue<QColor>(color), NodeTypeModel::ColorRole);
}

void DocumentTypesWidget::onNodeTypeNameChanged(const QModelIndex& index, const QString& name)
{
    m_nodeTypeModel.setData(index, QVariant(name), NodeTypeModel::TitleRole);
}

void DocumentTypesWidget::onCreateEdgeType()
{
    NodeTypePtr type = NodeType::create(m_document);
    type->setName(i18n("unnamed"));
}
