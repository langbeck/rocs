/***************************************************************************
 *
 * This file is part of the KDE project
 * copyright (C)2004-2007 by Tomaz Canabrava (tomaz.canabrava@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "QtScriptBackend.h"
#include "DataStructure.h"
#include "Data.h"
#include <KDebug>
#include <KTextBrowser>
#include "Document.h"
#include <QScriptEngineDebugger>
#include <unistd.h> // usleep
#include <ToolsPluginInterface.h>

static QtScriptBackend *self;

static QScriptValue debug_script(QScriptContext* context, QScriptEngine* /*engine*/) {
    self->debug(QString("%1 \n").arg(context->argument(0).toString()));
    return QScriptValue();
}

static QScriptValue output_script(QScriptContext *context, QScriptEngine* /*engine*/){
    self->output(QString("%1 \n").arg(context->argument(0).toString()));
    return QScriptValue();
}

void QtScriptBackend::stop(){
      if (!_engine) return;

      if (_engine->isEvaluating()){
        _engine->abortEvaluation();
      }
      _engine->deleteLater();
      _engine = 0;
      emit finished();
}

void QtScriptBackend::start()
{
    stop();

    _engine = new QScriptEngine();
    emit engineCreated(_engine);

    _engine->globalObject().setProperty("debug",  engine()->newFunction(debug_script));
    _engine->globalObject().setProperty("output", engine()->newFunction(output_script));

    int size = _document->dataStructures().size();
    for (int i = 0; i < size; i++) {
        _document->dataStructures().at(i)->setEngine(_engine);
    }
    createGraphList();
    _engine->setProcessEventsInterval(100); //! TODO: Make that changable.
    
    QString error = _engine->evaluate(_script).toString();

    emit finished();
    emit sendDebug(error);
}

bool QtScriptBackend::isRunning(){
  if ((_engine) && (_engine->isEvaluating())){
        return true;
  }
  return _runningTool;
}

QtScriptBackend::QtScriptBackend(QObject* parent): AbstractRunBackend(parent){
    self = this;
    _engine = 0;
    _runningTool = false;
}


void QtScriptBackend::runTool(ToolsPluginInterface * plugin, Document *graphs){
    _runningTool = true;
    _document = graphs;
    _script = plugin->run(graphs);
    if ( !_script.isEmpty()){
      start();
    }
    _runningTool = false;
}

void QtScriptBackend::createGraphList() {
    QScriptValue graphList = _engine->newArray();
    _engine->globalObject().setProperty("graphs", graphList);

    // Add all the graphs on the array as an array, and if it has a name,
    // also add it for direct acess with it's name.
    int size = _document->dataStructures().size();
    for (int i = 0; i < size; i++) {
        graphList.property("push").call(graphList, QScriptValueList() << _document->dataStructures().at(i)->scriptValue());
    }
}

