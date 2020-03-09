#include "puzzlecalculator.h"
#include "model/node.h"
#include "model/connection.h"
#include "model/component.h"

PuzzleCalculator::PuzzleCalculator()
{
}

/*!
 * \brief Ruft das Berechnen des Widerstandes auf.
 *
 * \param   connections
 * \param   components
 * \return Gibt den Widerstandswert als double zurück.
 *
 *
 */
double PuzzleCalculator::calculate(QList<Connection*> connections, QList<Component*> components)
{
    _connections = connections;
    _components = components;

    QList<Node*> nodes;
    QList<RowPiece> rowPieces = findRowPieces(nodes);

    if (!rowPieces.isEmpty() && isNodeConnectedToPowerSupply(rowPieces))
    {
        _resistanceValue = calculateResistanceValueFromRowPieces(rowPieces, nodes);
        qDebug() << "Widerstandswert Calculate" << _resistanceValue;
        calculateVoltageAndAmp();
        for(RowPiece rowpiece : _mergeList)
        {
            qDebug() << rowpiece.getAmp() << "Ampere";
            for(Component* component : rowpiece.getComponents())
            {
                qDebug() <<"moin" << component->getName() << rowpiece.getIsMergedParallel() << rowpiece.getAmp();
            }
            qDebug() <<"___________________";
        }
        return _resistanceValue;
    }
    return 0.0;
}

void PuzzleCalculator::setLists(QList<Connection*> connections, QList<Component*> components)
{
    _connections = connections;
    _components = components;
}

QList<RowPiece> PuzzleCalculator::findRowPieces(QList<Node*>& nodes)
{
    //Anfang suchen
    QList<ComponentPort> startOfSearchComponentsPorts = findFirstComponentPort();
    bool foundPowerSupply = (!startOfSearchComponentsPorts.isEmpty());

    QList<RowPiece> rowPieces;
    if (foundPowerSupply)
    {
        //Beginn der rekursiven Suche nach RowPieces
        bool analysisHasEndedSuccessful = true;
        for (ComponentPort componentPort : startOfSearchComponentsPorts)
        {
            qDebug() << "BeginnComponent" << componentPort.getComponent()->getName() << "Port"
                     << componentPort.getPort();
            pathAnalysis(componentPort, analysisHasEndedSuccessful, &rowPieces, &nodes);
        }

        if (analysisHasEndedSuccessful)
        {
            //test
            for (RowPiece rp : rowPieces)
            {
                qDebug() << "Widerstand RowPiece" << rp.getResistanceValue() << " von " << rp.getNodeOne()->getId()
                         << " nach " << rp.getNodeTwo()->getId();
            }
            for (Node* n : nodes)
            {
                QString comps = "";
                for (ComponentPort cp : n->getComponentPorts())
                {
                    comps += (cp.getComponent()->getName() + " ");
                }
                qDebug() << "Node " << n->getId() << comps;
            }
        }
    }
    return rowPieces;
}

void PuzzleCalculator::findSameRowPieces(RowPiece rowpiece)
{
    for(RowPiece& rowpiece2 : _mergeList)
    {
        if(rowpiece.getComponents() == rowpiece2.getComponents())
        {
            qDebug() << "gefunden" << rowpiece2.getAmp() << rowpiece.getAmp();
            rowpiece2.setAmp(rowpiece.getAmp());
        }
    }
}

void PuzzleCalculator::searchingForIndirectParallelNeighbours(QList<ComponentPort>& foundComponentPorts)
{
    QList<ComponentPort> newFound;

    for (ComponentPort cp : foundComponentPorts)
    {
        for (Connection* con : _connections)
        {
            if (cp == con->getComponentPortOne() && !foundComponentPorts.contains(con->getComponentPortTwo()))
            {
                newFound.append(con->getComponentPortTwo());
            }
            else if (cp == con->getComponentPortTwo() && !foundComponentPorts.contains(con->getComponentPortOne()))
            {
                newFound.append(con->getComponentPortOne());
            }
        }
    }
    if (newFound.count() != 0)
    {
        for (ComponentPort cp : newFound)
        {
            foundComponentPorts.append(cp);
        }
        //nur die neu gefundenen Komponenten werden auf Nachbarn untersucht
        searchingForIndirectParallelNeighbours(foundComponentPorts);
    }
}

void
PuzzleCalculator::searchingForDirectParallelNeighbours(ComponentPort actualComPort,
                                                       QList<ComponentPort>& foundComponentPorts)
{
    for (Connection* c : _connections)
    {
        if (actualComPort == c->getComponentPortOne())
        {
            if (!foundComponentPorts.contains(c->getComponentPortTwo()))
            {
                foundComponentPorts.append(c->getComponentPortTwo());
            }
        }
        else if (actualComPort == c->getComponentPortTwo())
        {
            if (!foundComponentPorts.contains(c->getComponentPortOne()))
            {
                foundComponentPorts.append(c->getComponentPortOne());
            }
        }
    }
    searchingForIndirectParallelNeighbours(foundComponentPorts);

    //Aus der Liste wird die Dopplung des ActualComponents entfernt
    foundComponentPorts.removeAll(actualComPort);
}

QList<ComponentPort> PuzzleCalculator::findFirstComponentPort(void)
{
    ComponentPort startOfSearch(nullptr, Component::Port::null);
    for (Connection* connection : _connections)
    {
        if (Component::ComponentType::PowerSupply ==
                connection->getComponentPortOne().getComponent()->getComponentTypeInt())
        {
            startOfSearch = connection->getComponentPortOne();
            break;
        }
        else if (Component::ComponentType::PowerSupply ==
                 connection->getComponentPortTwo().getComponent()->getComponentTypeInt())
        {
            startOfSearch = connection->getComponentPortTwo();
            break;
        }
    }

    bool foundStartOfSearch = (startOfSearch.getComponent() != nullptr);
    if (!foundStartOfSearch)
    {
        return QList<ComponentPort>();
    }
    return searchForNeighbours(startOfSearch);
}

void PuzzleCalculator::pathAnalysis(ComponentPort actualComponentPort, bool& hasAnalysisEndedSuccessful,
                                    QList<RowPiece>* rowPieces, QList<Node*>* knownNodes)
{
    //Wenn der Knoten bereits bekannt ist, ist die Abbruchbedingung erreicht
    //TODO: NodeIsKnown entfernen
    bool nodeIsKnown = false;

    QList<Component*> rowPiecesComponents;
    rowPiecesComponents.append(actualComponentPort.getComponent());

    //Widerstand des RowPieces
    int resistanceValueOfRowPiece = 0;

    /*
         * Finde alle ComponentPorts, die mit dem gegenüberliegenden Port von actualComponentPort verbunden sind.
         * So kann zwischen einer Reihen- und Parallelschaltung unterschieden werden.
         */
    QList<ComponentPort> neighbourComponentPortsOfStartNode = searchForNeighbours(actualComponentPort);
    Node* startNode = getOrCreateNode(actualComponentPort, neighbourComponentPortsOfStartNode, nodeIsKnown, knownNodes);

    QList<ComponentPort> neighbourComponentPorts = searchForNeighbours(actualComponentPort.getOppisiteComponentPort());

    //der aktuelle ComponentPort muss gedreht werden, um den Endknoten bestimmen zu können
    actualComponentPort.invertPort();
    bool neighbourComponentPortsContainPowerSupply = isPowerSupplyinComponentPortList(neighbourComponentPorts);

    /*
         * Solange es eine Reihenschaltung ist und man nicht bei der Spannungsquelle wieder angekommen ist,
         * wird der Widerstandswert der Komponenten addiert
         */
    resistanceValueOfRowPiece += actualComponentPort.getComponent()->getValue();
    while (neighbourComponentPorts.count() == 1 && !neighbourComponentPortsContainPowerSupply)
    {
        //der Widerstandswert des RowPieces wird addiert
        resistanceValueOfRowPiece += neighbourComponentPorts.last().getComponent()->getValue();
        //der nächste Widerstand wird betrachtet, damit Endknoten bestimmt werden kann
        actualComponentPort = neighbourComponentPorts.last().getOppisiteComponentPort();
        qDebug() << "actualComponentPort" << actualComponentPort.getComponent()->getName()
                 << actualComponentPort.getPort();
        rowPiecesComponents.append(actualComponentPort.getComponent());
        //die mit dem nächsten Widerstand verbundenen Widerstände werden gesucht
        neighbourComponentPorts = searchForNeighbours(actualComponentPort);
        neighbourComponentPortsContainPowerSupply = isPowerSupplyinComponentPortList(neighbourComponentPorts);
    }

    if (0 == neighbourComponentPorts.count())
    {
        //eine offene Verbindung wurde gefunden und hat den Wert ungültig gemacht
        qDebug() << "offene Verbindung";
        hasAnalysisEndedSuccessful = false;
        return;
    }
    else
    {
        //finde oder erstelle den Knoten für das Ende
        bool nodeIsKnown = true;
        Node* endNode = getOrCreateNode(actualComponentPort, neighbourComponentPorts, nodeIsKnown, knownNodes);
        //erstelle das neue RowPiece
        RowPiece rowPiece(startNode, endNode, resistanceValueOfRowPiece, rowPiecesComponents);

        //Abbruchbedienung das gefundene RowPieces, darf nicht bekannt sein
        bool isAlreadyKnownRowPiece = rowPieces->contains(rowPiece);
        if (isAlreadyKnownRowPiece)
        {
            qDebug() << "Abbruchbedingus" << actualComponentPort.getComponent()->getName() << startNode->getId()
                     << rowPiecesComponents.first()->getName();
        }
        if (!isAlreadyKnownRowPiece)
        {
            rowPieces->append(rowPiece);

            //Überprüfung: ist eine Spannungsquelle unter den verbundenen Komponenten (Abbruchbedingung)
            bool neighbourComponentPortsContainPowerSupply = isPowerSupplyinComponentPortList(neighbourComponentPorts);
            qDebug() << "found power supply " << neighbourComponentPortsContainPowerSupply
                     << neighbourComponentPorts.count();

            //Wenn die PowerSupply gefunden wurde, wird die Rekursion beendet
            if (!neighbourComponentPortsContainPowerSupply)
            {
                //führe diese Methode für jeden verbundenen Widerstand durch
                for (ComponentPort componentPort : neighbourComponentPorts)
                {
                    pathAnalysis(componentPort, hasAnalysisEndedSuccessful, rowPieces, knownNodes);
                }
            }
        }
    }

}

Node* PuzzleCalculator::getOrCreateNode(ComponentPort componentPortForNewNode,
                                        QList<ComponentPort> connectedComponentPorts,
                                        bool& nodeIsKnown, QList<Node*>* knownNodes)
{
    for (Node* node : *knownNodes)
    {
        nodeIsKnown = node->getComponentPorts().contains(componentPortForNewNode);
        if (nodeIsKnown)
        {
            return node;
        }
    }

    /*
     * componentPortForNewNode ist mit keinem bekannten Node verbunden.
     * Daher wird ein neuer Node erstellt
     */
    connectedComponentPorts.append(componentPortForNewNode);
    bool isConnectedToPowerSupply = false;
    for(ComponentPort cp : connectedComponentPorts)
    {
        if(Component::ComponentType::PowerSupply == cp.getComponent()->getComponentTypeInt())
        {
            isConnectedToPowerSupply = true;
            break;
        }
    }
    Node* createdNode = new Node(knownNodes->count(), connectedComponentPorts, isConnectedToPowerSupply);
    knownNodes->append(createdNode);
    return createdNode;
}

QList<ComponentPort> PuzzleCalculator::searchForNeighbours(ComponentPort componentPortForSearch)
{
    QList<ComponentPort> neighbours;
    searchingForDirectParallelNeighbours(componentPortForSearch, neighbours);
    return neighbours;
}

bool PuzzleCalculator::isPowerSupplyinComponentPortList(const QList<ComponentPort> list)
{
    for (ComponentPort componentPort : list)
    {
        if (Component::ComponentType::PowerSupply == componentPort.getComponent()->getComponentTypeInt())
        {
            return true;
        }
    }
    return false;
}

bool PuzzleCalculator::isNodeConnectedToPowerSupply(QList<RowPiece> rowPieces)
{
    QList<Node*> nodes;
    for(RowPiece rp : rowPieces)
    {
        if(rp.getNodeOne()->isConnectedToPowerSupply())
        {
            if(!nodes.contains(rp.getNodeOne()))
            {
                nodes.append(rp.getNodeOne());
            }
        }
        if(rp.getNodeTwo()->isConnectedToPowerSupply())
        {
            if(!nodes.contains(rp.getNodeTwo()))
            {
                nodes.append(rp.getNodeTwo());
            }
        }
    }
    return (nodes.count() == 2);
}

QList<RowPiece> PuzzleCalculator::calculateStar(RowPiece rowPieceA, RowPiece rowPieceB, RowPiece rowPieceC, Node* newNode)
{
    qDebug() << "RowA" << rowPieceA.getComponents()[0]->getName();
    qDebug() << "RowB" << rowPieceB.getComponents()[0]->getName();
    qDebug() << "RowC" << rowPieceC.getComponents()[0]->getName();

    Node* equalNodeA = rowPieceA.getEqualNode(rowPieceB);
    Node* equalNodeB = rowPieceB.getEqualNode(rowPieceC);
    Node* equalNodeC = rowPieceC.getEqualNode(rowPieceA);
    qDebug() << "Eq NodeA" << equalNodeA->getId();
    qDebug() << "Eq NodeB" << equalNodeB->getId();
    qDebug() << "Eq NodeC" << equalNodeC->getId();

    //Summe aller Widerstände
    int additionValue = rowPieceA.getResistanceValue() + rowPieceB.getResistanceValue() + rowPieceC.getResistanceValue();

    // Die drei Stern-Zweige
    double starValueA = ((rowPieceA.getResistanceValue() * rowPieceB.getResistanceValue()) / additionValue);
    double starValueB = ((rowPieceB.getResistanceValue() * rowPieceC.getResistanceValue()) / additionValue);
    double starValueC = ((rowPieceA.getResistanceValue() * rowPieceC.getResistanceValue()) / additionValue);

    RowPiece rowPieceAB(newNode, equalNodeA, starValueA, rowPieceA.getComponents());
    RowPiece rowPieceBC(newNode, equalNodeB, starValueB, rowPieceB.getComponents());
    RowPiece rowPieceCA(equalNodeC, newNode, starValueC, rowPieceC.getComponents());

    QList<RowPiece> listOfNewRowPieces;
    listOfNewRowPieces.append(rowPieceAB);
    listOfNewRowPieces.append(rowPieceBC);
    listOfNewRowPieces.append(rowPieceCA);
    return listOfNewRowPieces;
}

void PuzzleCalculator::calculateVoltageAndAmp()
{
    int ListSize = _mergeList.count() - 1;
    double totalCurrent = 0.0;
    if(_mergeList.isEmpty())
    {
        for(Component* component : _components)
        {
            if(1 == component->getComponentTypeInt())
            {
                component->setAmp(component->getVoltage() / _resistanceValue);
                totalCurrent = component->getAmp();
            }
        }
        for(Component* component : _components)
        {
            if(0 == component->getComponentTypeInt())
            {
                component->setAmp(totalCurrent);
                component->setVoltage(totalCurrent * component->getValue());
            }
        }
    }
    else
    {
    for(Component* component : _components)
    {
        if(1 == component->getComponentTypeInt())
        {
            component->setAmp(component->getVoltage() / _resistanceValue);
            _mergeList[ListSize].setAmp(component->getAmp());
        }
    }
    for(int i = 0; i < (ListSize +1) / 3; i++)
    {
    if(_mergeList[ListSize - 3*i].getIsMergedParallel())
    {
        qDebug() << "yooo";
        RowPiece* mergedRowpieces = &_mergeList[ListSize - 3*i];
        RowPiece* rowpiece = &_mergeList[ListSize - 3*i - 1];
        RowPiece* rowpiece2 = &_mergeList[ListSize - 3*i -2];

        rowpiece->setAmp(mergedRowpieces->getAmp() * (rowpiece2->getResistanceValue() / (rowpiece->getResistanceValue() + rowpiece2->getResistanceValue())));
        findSameRowPieces(*rowpiece);
        rowpiece2->setAmp(mergedRowpieces->getAmp() * (rowpiece->getResistanceValue() / (rowpiece->getResistanceValue() + rowpiece2->getResistanceValue())));
        findSameRowPieces(*rowpiece2);
    }
    else
    {
        RowPiece* mergedRowpieces = &_mergeList[ListSize - 3*i];
        RowPiece* rowpiece = &_mergeList[ListSize - 3*i - 1];
        RowPiece* rowpiece2 = &_mergeList[ListSize - 3*i - 2];

        rowpiece->setAmp(mergedRowpieces->getAmp());
        findSameRowPieces(*rowpiece);
        rowpiece2->setAmp(mergedRowpieces->getAmp());
        findSameRowPieces(*rowpiece2);
        bool hallo = false;
        if(_mergeList[ListSize - 2].getAmp() == _mergeList[ListSize - 3].getAmp())
        {
            hallo = true;
        }
        qDebug() << hallo << "kaka" << _mergeList[ListSize - 2].getAmp() << _mergeList[ListSize - 3].getAmp();
    }
    }
    }
}

double PuzzleCalculator::calculateResistanceValueFromRowPieces(QList<RowPiece> rowPieces, QList<Node*> nodes)
{
    //_rowPieceListForCalculateAmp = rowPieces;
    while (1 < rowPieces.count())
    {
        for (RowPiece rp : rowPieces)
        {
            QString string = "";
            for (Component* cp : rp.getComponents())
            {
                string += cp->getName();
            }
            qDebug() << "RowPiece" << string;
        }
        qDebug() << "---------------------------------";

        bool changedSomething = false;
        for (RowPiece& rowPieceA : rowPieces)
        {
            //Widerstand kurzgeschlossen
            if(rowPieceA.getNodeOne() == rowPieceA.getNodeTwo())
            {
                rowPieces.removeOne(rowPieceA);
                break;
            }
            for (RowPiece& rowPieceB : rowPieces)
            {
                if (rowPieceA != rowPieceB && rowPieceA.hasEqualNodesOnBothSides(rowPieceB))
                {
                    _mergeList.append(rowPieceA);
                    _mergeList.append(rowPieceB);
                    rowPieceA.parallelMerge(rowPieceB);
                    rowPieceA.setIsMergedParallel(true);
                    _mergeList.append(rowPieceA);
                    //_rowPieceListForCalculateAmp.append(rowPieceA);
                    rowPieces.removeOne(rowPieceB);
                    changedSomething = true;
                    break;
                }
                else if (rowPieceA != rowPieceB && rowPieceA.hasOneEqualNode(rowPieceB))
                {
                    Node* equalNode = rowPieceA.getEqualNode(rowPieceB);
                    if (equalNode != nullptr && !equalNode->isConnectedToPowerSupply())
                    {
                        if(changedSomething)
                        {
                            break;
                        }

                        if (2 == countNodesInRowPieces(equalNode, rowPieces))
                        {
                            _mergeList.append(rowPieceA);
                            _mergeList.append(rowPieceB);
                            rowPieceA.rowMerge(rowPieceB);
                            _mergeList.append(rowPieceA);
                            rowPieces.removeOne(rowPieceB);
                            changedSomething = true;
                            break;
                        }
                        else if (1 == countNodesInRowPieces(equalNode, rowPieces))
                        {
                            qDebug() << "nur ein RowPiece mit bestimmtem Node";
                        }
                    }

                }
                if (changedSomething)
                {
                    break;
                }
            }
            if (changedSomething)
            {
                break;
            }
        }

        for(RowPiece& rowPieceA : rowPieces)
        {
            for(RowPiece& rowPieceB : rowPieces)
            {
                if (rowPieceA != rowPieceB && rowPieceA.hasOneEqualNode(rowPieceB))
                {
                    Node* equalNode = rowPieceA.getEqualNode(rowPieceB);
                    if (equalNode != nullptr && !equalNode->isConnectedToPowerSupply())
                    {
                        //Wenn die Anzahl der Node ComponentPort größer 3 ist, klingt es nach Stern-Dreieck, RowPieces werden nach Anbindung zum Node
                        //durchsucht
                        int count = 0;
                        for(RowPiece rs : rowPieces)
                        {
                            if(rs.getNodeOne()->getId() == equalNode->getId())
                            {
                                count++;
                            }
                            else if(rs.getNodeTwo()->getId() == equalNode->getId())
                            {
                                count++;
                            }
                        }
                        if(count >= 3 && !changedSomething)
                        {
                            //Nun schauen auf der gegenüberliegenden Seite vom gleichen Node
                            Node* oppositeNode = rowPieceB.getOppositeNode(equalNode);
                            RowPiece* searchedRowPieces = nullptr;
                            bool found = false;

                            /*Zuerst wird geschaut, ob auch am Opposite Node mindestens 3 RowPieces angeschlossen sind
             * Durchgehen aller ComponentPorts des gegenüberliegenden Nodes, schauen, wo der Component teil
            * eines RowPieces ist, wenn eine angrenzendes RowPieces gefunden wurde, schauen, ob der gegenüberliegende Node gleich
            * dem anfangs gefundenen EqualNode ist, wenn ja rausspringen
            */
                            int count = 0;
                            for(RowPiece rs : rowPieces)
                            {
                                if(rs.getNodeOne()->getId() == oppositeNode->getId())
                                {
                                    count++;
                                }
                                else if(rs.getNodeTwo()->getId() == oppositeNode->getId())
                                {
                                    count++;
                                }
                            }
                            if(count >= 3)
                            {
                                for(ComponentPort cp :  oppositeNode->getComponentPorts())
                                {
                                    for(RowPiece rs : rowPieces)
                                    {
                                        for(Component* c : rs.getComponents())
                                        {
                                            if(c == cp.getComponent())
                                            {
                                                if(rs.getOppositeNode(oppositeNode) == rowPieceA.getOppositeNode(equalNode))
                                                    searchedRowPieces = new RowPiece(rs.getNodeOne(), rs.getNodeTwo(), rs.getResistanceValue(), rs.getComponents());
                                                break;
                                            }
                                        }
                                        if(found){break;}
                                    }
                                    if(found){break;}
                                }

                                if(oppositeNode != nullptr && searchedRowPieces != nullptr && *searchedRowPieces != rowPieceA && *searchedRowPieces != rowPieceB)
                                {
                                    //rp ist rowPieceC
                                    //Dreieck zu Stern
                                    QList<ComponentPort> componentPorts;
                                    Node* newNode = new Node(nodes.count(), componentPorts, false);
                                    nodes.append(newNode);

                                    //Erstellen der Stern-RowPieces und alte removen
                                    QList<RowPiece> listOfNewRowPieces = calculateStar(rowPieceA, rowPieceB, *searchedRowPieces, newNode);
                                    rowPieces.removeOne(rowPieceA);
                                    rowPieces.removeOne(rowPieceB);
                                    rowPieces.removeOne(*searchedRowPieces);
                                    rowPieces.append(listOfNewRowPieces);
                                    changedSomething = true;

                                    for(RowPiece r : listOfNewRowPieces)
                                    {
                                        qDebug() << "List of RowPiece" << r.getResistanceValue() << "Von" << r.getNodeOne()->getId() << "nach" << r.getNodeTwo()->getId();
                                    }
                                    qDebug() << "SternDreieck";
                                }
                            }
                        }

                    }
                }
                if(changedSomething)
                {
                    break;
                }
            }
            if(changedSomething)
            {
                break;
            }
        }
    }
    return rowPieces.last().getResistanceValue();
}

int PuzzleCalculator::countNodesInRowPieces(Node* nodeToCount, QList<RowPiece> listOfRowPieces)
{
    int counter = 0;
    for (RowPiece rowPiece : listOfRowPieces)
    {
        if (rowPiece.hasNode(nodeToCount))
        {
            counter++;
        }
    }
    return counter;
}
