/*
    This file is part of Rocs.
    Copyright 2011       Tomaz Canabrava <tomaz.canabrava@gmail.com>
    Copyright 2011       Wagner Reck <wagner.reck@gmail.com>
    Copyright 2011-2012  Andreas Cord-Landwehr <cola@uni-paderborn.de>

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

#undef QT_STRICT_ITERATORS // boost property map can't work with iterators being classes

#include "GraphStructure.h"
#include "KDebug"
#include "Data.h"
#include "Pointer.h"
#include "Document.h"
#include "DataStructure.h"
#include <KMessageBox>
#include "GraphNode.h"
#include "QtScriptBackend.h"

#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_concepts.hpp>

#include <KLocale>
#include <QString>

#include <cmath>
#include <utility>

DataStructurePtr Rocs::GraphStructure::create(Document *parent)
{
    return DataStructure::create<GraphStructure>(parent);
}

DataStructurePtr Rocs::GraphStructure::create(DataStructurePtr other, Document *parent)
{
    boost::shared_ptr<GraphStructure> ds = boost::static_pointer_cast<GraphStructure>(Rocs::GraphStructure::create(parent));

    ds->importStructure(other);
    return ds;
}

Rocs::GraphStructure::GraphStructure(Document* parent) :
    DataStructure(parent)
{
    _type = Graph;
}

void Rocs::GraphStructure::importStructure(DataStructurePtr other)
{
    //FIXME this import does not correctly import different types
    setGraphType(Graph);
    QHash <Data*, DataPtr> dataTodata;
    //FIXME only default data type considered
    foreach(DataPtr n, other->dataList(0)) {
        DataPtr newdata = createData("", 0); //n->name());
        newdata->setColor(n->color());
        newdata->setProperty("value", n->property("value").toString());
        newdata->setX(n->x());
        newdata->setY(n->y());
        newdata->setWidth(n->width());
        dataTodata.insert(n.get(), newdata);
    }
    //FIXME only default pointer type considered
    foreach(PointerPtr e, other->pointers(0)) {
        DataPtr from =  dataTodata.value(e->from().get());
        DataPtr to =  dataTodata.value(e->to().get());

        PointerPtr newPointer = createPointer(from, to, 0);
        if (newPointer.get()){
            newPointer->setColor(e->color());
            newPointer->setProperty("value", e->property("value").toString());
        }
    }
}

Rocs::GraphStructure::~GraphStructure()
{
}

QScriptValue Rocs::GraphStructure::nodes()
{
    QScriptValue array = engine()->newArray();
    foreach(int type, document()->dataTypeList()) {
    foreach(DataPtr n, dataList(type)) {
        array.property("push").call(array, QScriptValueList() << n->scriptValue());
    }
    }
    return array;
}

QScriptValue Rocs::GraphStructure::nodes(int type)
{
    QScriptValue array = engine()->newArray();
    foreach(DataPtr n, dataList(type)) {
        array.property("push").call(array, QScriptValueList() << n->scriptValue());
    }
    return array;
}

QScriptValue Rocs::GraphStructure::edges()
{
    QScriptValue array = engine()->newArray();
    foreach(int type, document()->pointerTypeList()) {
    foreach(PointerPtr n, pointers(type)) {
        array.property("push").call(array, QScriptValueList() << n->scriptValue());
    }
    }
    return array;
}

QScriptValue Rocs::GraphStructure::edges(int type)
{
    QScriptValue array = engine()->newArray();
    foreach(PointerPtr n, pointers(type)) {
        array.property("push").call(array, QScriptValueList() << n->scriptValue());
    }
    return array;
}

QScriptValue Rocs::GraphStructure::createNode()
{
    return createNode(0);
}

QScriptValue Rocs::GraphStructure::createNode(int type)
{
    DataPtr n = createData("", type);
    n->setEngine(engine());
    return n->scriptValue();
}

QScriptValue Rocs::GraphStructure::createEdge(Data* fromRaw, Data* toRaw)
{
    return createEdge(fromRaw, toRaw, 0);
}

QScriptValue Rocs::GraphStructure::createEdge(Data* fromRaw, Data* toRaw, int type)
{
    if (fromRaw == 0 || toRaw == 0) {
        qCritical() << "No edge added: data does not exist";
        emit scriptError(i18n("Cannot create edge: nodes are not defined."));
        return QScriptValue();
    }
    if (!document()->pointerTypeList().contains(type)) {
        emit scriptError(i18n("Cannot create edge: pointer type %1 not defined", type));
        return QScriptValue();
    }

    DataPtr from = fromRaw->getData();
    DataPtr to = toRaw->getData();

    PointerPtr edge = createPointer(from, to, type);
    if (edge) {
        edge->setEngine(engine());
        return edge->scriptValue();
    }
    qCritical() << "Could not add pointer to data structure";

    return QScriptValue();
}

QScriptValue Rocs::GraphStructure::overlay_edges(int overlay)
{
    emit scriptError(i18n("The global method \"%1\" is deprecated, please use \"%2\" instead.",
        QString("overlay_edges(int type)"),
        QString("edges(int type)")));
    return edges(overlay);
}

QScriptValue Rocs::GraphStructure::list_nodes()
{
    emit scriptError(i18n("The global method \"%1\" is deprecated, please use \"%2\" instead.",
        QString("list_nodes()"),
        QString("nodes()")));
    return nodes();
}

QScriptValue Rocs::GraphStructure::list_nodes(int type)
{
    emit scriptError(i18n("The global method \"%1\" is deprecated, please use \"%2\" instead.",
        QString("list_nodes(int type)"),
        QString("nodes(int type)")));
    return nodes(type);
}

QScriptValue Rocs::GraphStructure::list_edges()
{
    emit scriptError(i18n("The global method \"%1\" is deprecated, please use \"%2\" instead.",
        QString("list_edges()"),
        QString("edges()")));
    return edges();
}

QScriptValue Rocs::GraphStructure::list_edges(int type)
{
    emit scriptError(i18n("The global method \"%1\" is deprecated, please use \"%2\" instead.",
        QString("list_edges(int type)"),
        QString("edges(int type)")));
    return edges(type);
}

QScriptValue Rocs::GraphStructure::add_node(const QString& name)
{
    emit scriptError(i18n("The global method \"%1\" is deprecated, please use \"%2\" instead.",
        QString("add_node(string name)"),
        QString("createNode()")));
    DataPtr n = createData(name, 0);
    n->setEngine(engine());
    return n->scriptValue();
}

QScriptValue Rocs::GraphStructure::add_edge(Data* fromRaw, Data* toRaw)
{
    emit scriptError(i18n("The global method \"%1\" is deprecated, please use \"%2\" instead.",
        QString("add_edge(from, to)"),
        QString("createEdge(node from, node to)")));
    return add_overlay_edge(fromRaw, toRaw, 0);
}

QScriptValue Rocs::GraphStructure::add_overlay_edge(Data* fromRaw, Data* toRaw, int overlay)
{
    emit scriptError(i18n("The global method \"%1\" is deprecated, please use \"%2\" instead.",
        QString("add_overlay_edge(from, to, overlay)"),
        QString("createEdge(node from, node to, int type)")));
    return createEdge(fromRaw, toRaw, overlay);
}

QScriptValue Rocs::GraphStructure::dijkstra_shortest_path(Data* fromRaw, Data* toRaw)
{
    if (fromRaw == 0 || toRaw == 0) {
        return QScriptValue();
    }
    DataPtr from = fromRaw->getData();
    DataPtr to = toRaw->getData();

    QMap<DataPtr,PointerList> shortestPaths = dijkstraShortestPaths(from);
    QScriptValue pathEdges = engine()->newArray();
    foreach (PointerPtr edge, shortestPaths[to]) {
        pathEdges.property("push").call(
            pathEdges,
            QScriptValueList() << edge->scriptValue()
        );
    }
    return pathEdges;
}

QScriptValue Rocs::GraphStructure::distances(Data* fromRaw)
{
    if (fromRaw == 0) {
        return QScriptValue();
    }
    DataPtr from = fromRaw->getData();

    QMap<DataPtr,PointerList> shortestPaths = dijkstraShortestPaths(from);
    QScriptValue distances = engine()->newArray();
    foreach (DataPtr target, dataListAll()) {
        qreal length = 0;

        if (shortestPaths[target].isEmpty() && from != target) {
            length = INFINITY;
        } else {
            foreach (PointerPtr edge, shortestPaths[target]) {
                if (!edge->property("value").toString().isEmpty()) {
                    length += edge->property("value").toDouble();
                } else {
                    length += 1;
                }
            }
        }

        distances.property("push").call(
            distances,
            QScriptValueList() << length
        );
    }
    return distances;
}

QMap<DataPtr,PointerList> Rocs::GraphStructure::dijkstraShortestPaths(DataPtr from)
{
    // use copies of these lists to be safe agains changes of
    // data/pointer lists while computing shortest paths
    DataList dataListAll = this->dataListAll();
    PointerList pointerListAll = this->pointerListAll();

    if (!from) {
        return QMap<DataPtr,PointerList>();
    }

    typedef boost::adjacency_list < boost::listS, boost::vecS, boost::directedS,
            boost::no_property, boost::property <boost::edge_weight_t, qreal> > graph_t;
    typedef boost::graph_traits <graph_t>::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits <graph_t>::edge_descriptor edge_descriptor;
    typedef std::pair<int, int> Edge;

    // create IDs for all nodes
    QMap<int, int> node_mapping;
    QMap<std::pair<int, int>, PointerPtr> edge_mapping; // to map all edges back afterwards
    int counter = 0;
    foreach(DataPtr data, dataListAll) {
        node_mapping[data->identifier()] = counter++;
    }

    // use doubled size for case of undirected edges
    QVector<Edge> edges(pointerListAll.count() * 2);
    QVector<qreal> weights(pointerListAll.count() * 2);

    counter = 0;
    foreach(PointerPtr p, pointerListAll) {
        edges[counter] = Edge(node_mapping[p->from()->identifier()], node_mapping[p->to()->identifier()]);
        edge_mapping[std::make_pair<int&,int&>(node_mapping[p->from()->identifier()], node_mapping[p->to()->identifier()])] = p;
        if (!p->property("value").toString().isEmpty()) {
            weights[counter] = p->property("value").toDouble();
        } else {
            weights[counter] = 1;
        }
        counter++;
        // if graph is directed, also add back-edges
        if (p->direction() == PointerType::Bidirectional) {
            edges[counter] = Edge(node_mapping[p->to()->identifier()], node_mapping[p->from()->identifier()]);
            edge_mapping[std::make_pair<int&,int&>(node_mapping[p->to()->identifier()], node_mapping[p->from()->identifier()])] = p;
            if (!p->property("value").toString().isEmpty()) {
                weights[counter] = p->property("value").toDouble();
            } else {
                weights[counter] = 1;
            }
            counter++;
        }
    }

    // setup the graph
    graph_t g(edges.begin(),
              edges.end(),
              weights.begin(),
              dataListAll.count()
             );

    // compute Dijkstra
    vertex_descriptor source = boost::vertex(node_mapping[from->identifier()], g);
    QVector<vertex_descriptor> p(boost::num_vertices(g));
    QVector<int> dist(boost::num_vertices(g));
    boost::dijkstra_shortest_paths(g,
                                   source,
                                   boost::predecessor_map(p.begin()).distance_map(dist.begin())
                                  );

    // walk search tree and setup solution
    QMap<DataPtr,PointerList> shortestPaths = QMap<DataPtr,PointerList>();

    DataList::iterator toIter = dataListAll.begin();
    while (toIter != dataListAll.end()){
        PointerList path = PointerList();
        vertex_descriptor target = boost::vertex(node_mapping[(*toIter)->identifier()], g);
        vertex_descriptor predecessor = target;
        do {
            if (edge_mapping.contains(std::make_pair<int,int>(p[predecessor], predecessor))) {
                path.append(edge_mapping[std::make_pair<int,int>(p[predecessor], predecessor)]);
            }
            predecessor = p[predecessor];
        } while (p[predecessor] != predecessor);

        shortestPaths.insert((*toIter), path);
        ++toIter;
    }
    return shortestPaths;
}

void Rocs::GraphStructure::setGraphType(int type)
{
    if (_type == type) {
        return;
    }

    if (_type == Multigraph && type != _type) {
        if (KMessageBox::warningContinueCancel(0, i18n("This action will probably remove some edges. Do you want to continue?")) != KMessageBox::Continue) {
            return;
        }
    } else { // for switch to multigraph nothing has to be done
        _type = GRAPH_TYPE(type);
        return;
    }

    // need to convert multigraph to graph
    //FIXME only default data type considered
    foreach(DataPtr data, dataList(0)) {
        // Clear the rest. there should be only one edge between two nodes.
        foreach(DataPtr neighbor, data->adjacentDataList()) {
            if (data == neighbor) {
                continue;
            }
            while (data->pointerList(neighbor).count() > 1) {
                data->pointerList(neighbor).last()->remove();
            }
        }
    }
}

Rocs::GraphStructure::GRAPH_TYPE Rocs::GraphStructure::graphType() const
{
    return _type;
}

bool Rocs::GraphStructure::multigraph() const
{
    return (_type == Multigraph);
}

PointerPtr Rocs::GraphStructure::createPointer(DataPtr from, DataPtr to, int pointerType)
{
    bool directed = document()->pointerType(pointerType)->direction() == PointerType::Unidirectional;
    if (!directed && !multigraph()) {
        // do not add back-edges if graph is undirected
        foreach(PointerPtr pointer, from->pointerList(to)) {
            if (pointer->pointerType() == pointerType) {
                emit scriptError(i18n("Could not add back-edge (%1->%2) to undirected graph.", from->identifier(), to->identifier()));
                return PointerPtr();
            }
        }
    }

    if (!multigraph()) {     // do not add double edges
        PointerList list = from->outPointerList();
        foreach(PointerPtr tmp, list) {
            if (tmp->to() == to && tmp->pointerType() == pointerType) {
                emit scriptError(
                    i18n("Could not add existing edge (%1->%2): this graph is no multigraph.", from->identifier(), to->identifier()));
                return PointerPtr();
            }
        }
    }

    return DataStructure::createPointer(from, to, pointerType);
}

DataPtr Rocs::GraphStructure::createData(const QString& name, int dataType)
{
    if (readOnly()) {
        return DataPtr();
    }
    boost::shared_ptr<GraphNode> n = boost::static_pointer_cast<GraphNode>(
                                         GraphNode::create(getDataStructure(), generateUniqueIdentifier(), dataType)
                                     );
    n->setProperty("name", name);
    return addData(n);
}

QMap<QString, QString> Rocs::GraphStructure::pluginProperties() const
{
    QMap<QString,QString> properties = QMap<QString,QString>();
    properties.insert("type", QString("%1").arg(_type));
    return properties;
}

void Rocs::GraphStructure::setPluginProperty(const QString& identifier, const QString& property)
{
    if (identifier.startsWith(QLatin1String("type"))) {
        setGraphType(property.toInt());
    }
    else {
        qDebug() << "Skipping unknown graph structure property: " << identifier << " / " << property;
    }
}
