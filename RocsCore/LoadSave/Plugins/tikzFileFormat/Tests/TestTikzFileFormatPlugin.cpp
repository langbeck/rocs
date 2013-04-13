/*
    This file is part of Rocs.
    Copyright 2012       Andreas Cord-Landwehr <cola@uni-paderborn.de>

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

#include "TestTikzFileFormatPlugin.h"
#include "DataStructure.h"
#include "Data.h"
#include "Pointer.h"
#include "DocumentManager.h"
#include <qtest_kde.h>
#include "../TikzFileFormatPlugin.h"


TestTikzFileFormatPlugin::TestTikzFileFormatPlugin()
{
}

void TestTikzFileFormatPlugin::serializeTest()
{
    DocumentManager::self().addDocument(new Document("testSerialization"));
    Document* document = DocumentManager::self().activeDocument();
    QMap<QString, DataPtr> dataList;

    // Creates a simple Graph with 5 data elements and connect them with pointers.
    DataStructurePtr ds = document->activeDataStructure();
    ds->setProperty("name", "Graph1");
    dataList.insert("a", ds->addData("first node", 0));
    dataList.insert("b", ds->addData("b", 0));
    dataList.insert("c", ds->addData("c", 0));
    dataList.insert("d", ds->addData("d", 0));
    dataList.insert("e", ds->addData("e", 0));

    ds->createPointer(dataList["a"], dataList["b"], 0)->setProperty("value", "test value");
    ds->createPointer(dataList["b"], dataList["c"], 0);
    ds->createPointer(dataList["c"], dataList["d"], 0);
    ds->createPointer(dataList["d"], dataList["e"], 0);
    ds->createPointer(dataList["e"], dataList["a"], 0);

    // create exporter plugin
    TikzFileFormatPlugin serializer(this, QList<QVariant>());
    serializer.setFile(KUrl::fromLocalFile("test.tgf"));
    serializer.writeFile(*document);
    QVERIFY(serializer.hasError() == false);
}

QTEST_MAIN(TestTikzFileFormatPlugin);
