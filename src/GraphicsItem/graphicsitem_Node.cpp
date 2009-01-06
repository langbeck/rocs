/* This file is part of Rocs,
	 Copyright (C) 2008 by:
	 Tomaz Canabrava <tomaz.canabrava@gmail.com>
	 Ugo Sangiori <ugorox@gmail.com>

	 Rocs is free software; you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation; either version 2 of the License, or
	 (at your option) any later version.

	 Rocs is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
	 GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License
	 along with Step; if not, write to the Free Software
	 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA	02110-1301	USA
*/


#include "graphicsitem_Node.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>


#include "node.h"
#include "graph.h"
#include <KDebug>

NodeItem::NodeItem(Node *node, QGraphicsItem *parent)
		 : QObject(0), QGraphicsItem(parent)
{
		_node = node;
		QPointF pos( _node -> property("x").toDouble() ,_node->property("y").toDouble() );

		setPos( pos );
		setCacheMode(DeviceCoordinateCache);
		setZValue(1);
		setFlag(ItemIsSelectable);

	connect (_node, SIGNAL(removed()), this, SLOT(removed()));
}

 QRectF NodeItem::boundingRect() const
 {
		 return QRectF(-12, -12 , 25, 25);
 }

 QPainterPath NodeItem::shape() const
 {
		 QPainterPath path;
		 path.addEllipse(-10, -10, 20, 20);
		 return path;
 }

 void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
 {
		
		 painter->setPen(Qt::NoPen);

		 QColor color = _node->property("color").value<QColor>();
		 painter->setBrush( color );
		 painter->drawEllipse(-7, -7, 20, 20);

		 QRadialGradient gradient(-3, -3, 10);
		 if (option->state & QStyle::State_Sunken) {
				 gradient.setCenter(3, 3);
				 gradient.setFocalPoint(3, 3);
				 gradient.setColorAt(1, color.light(120));
				 gradient.setColorAt(0, color);
		 } else {
				 gradient.setColorAt(0, color.light(240));
				 gradient.setColorAt(1, color);
		 }
		 painter->setBrush(gradient);
		 painter->setPen(QPen(color, 2));
		 painter->drawEllipse(-10, -10, 20, 20);

		if (isSelected()){
			painter->setBrush(QBrush());
			painter->setPen(QPen(Qt::black));
			painter->drawRect(QRectF(-12 , -12 , 25 , 25 ));
		}
 }

void NodeItem::updatePos(QPointF pos){
	setPos( pos );
	update();
	foreach(QGraphicsItem* i, _edges){
		i->update();
	}
	_node->setProperty("x", pos.x());
	_node->setProperty("y", pos.y());
}

void NodeItem::updateName(const QString& ){}
void NodeItem::updateVisited(bool ){}
void NodeItem::updateValue(qreal ){}
void NodeItem::updateTotal(qreal ){}
void NodeItem::updateColor(QColor ){}

void NodeItem::removed(){
	kDebug() << " Not Implemented Yet " << "removed";
}

void NodeItem::addEdge(QGraphicsItem *e){
	_edges.append(e);
}
#include "graphicsitem_Node.moc"
