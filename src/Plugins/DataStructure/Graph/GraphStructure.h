/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef GRAPHSTRUCTURE_H
#define GRAPHSTRUCTURE_H

#include "DataStructure.h"

namespace Rocs{
class GraphStructure : public DataStructure {
  Q_OBJECT
  public:

    GraphStructure ( Document* parent = 0 );

    GraphStructure(DataStructure& other, Document *parent);

    virtual ~GraphStructure();
  public slots:
    QScriptValue list_nodes();
    QScriptValue list_edges();
    QScriptValue add_node(const QString& name);
    QScriptValue add_edge(Data* from, Data* to);
    QScriptValue node_byname(const QString& name);
//     QScriptValue begin_node();
//     QScriptValue end_nodes();
    void setDirected(bool directed);
private:
    bool _directed;
};
}
#endif // GRAPHSTRUCTURE_H