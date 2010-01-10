/***************************************************************************
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

#ifndef THREADSCRIPTEXECUTION_H
#define THREADSCRIPTEXECUTION_H

#include <QThread>
#include <QMutex>
class KTextBrowser;
class GraphDocument;
class QtScriptBackend;

class ThreadScriptExecution : public QThread{
  Q_OBJECT
  public:
    ThreadScriptExecution(KTextBrowser * debugView = 0);
    virtual ~ThreadScriptExecution();

  public slots:
    void run();
//    bool isEvaluating();
    void abort();
    void setData(QString script, GraphDocument * graphDocument);
    void debug(const QString& str);

  private:
    QtScriptBackend* _engine;

    QString _script;
    GraphDocument * _graphDocument;
    KTextBrowser * _txtDebug;
    QMutex _mutex;
};

#endif