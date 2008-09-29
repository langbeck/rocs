#include "SimpleGraph.h"
#include "SimpleNode.h"
#include "Edge.h"
#include "GraphCollection.h"

SimpleGraph::SimpleGraph(GraphCollection *parent) : Graph(parent)
{

}

SimpleGraph::~SimpleGraph()
{

}

Edge* SimpleGraph::createEdge(Node* from, Node* to)
{
  if (( _nodes.indexOf(from) == -1 ) || ( _nodes.indexOf(to) == -1) )
  {
    return 0;
  }
  if ( from->isConnected(to) )
  {
    return 0;
  }
  
  Edge *e = new Edge(from, to, this);

  from->addEdge(e);
  to->addEdge(e);

  _edges.append(e);
  emit edgeCreated(e);
  return e;
}

Node* SimpleGraph::createNode(QPointF position)
{
  Node *n = new SimpleNode(position, this);

  _nodes.append(n);

  n -> setIndex( _nodes.size() - 1 );

  emit nodeCreated(n);

  return n;
}
