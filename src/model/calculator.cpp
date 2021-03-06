#include <QDebug>
#include <utility>

#include "calculator.h"
#include "model/node.h"
#include "model/connection.h"
#include "model/component.h"
#include "model/resistor.h"

/*!
 * \brief Ruft das Berechnen des Widerstandes auf.
 *
 * \param   connections ist die Liste von Verbindungen des Netzwerkes
 * \param   components  ist die Liste von Kommponenten des Netzwerkes
 * \return Gibt den Widerstandswert als double zurück.
 *
 * Wenn die Spannungsquelle verbunden ist, kann der Gesamtwiderstandswert berechnet werden.
 */
long double Calculator::calculate(QList<Connection*> connections, QList<Component*> components)
{
    _hasUsedStarCalculation = false;
    _connections = connections;
    _components = initializeAmpAndVoltage(components);

    QList<RowPiece> mergeList;
    QList<Node*> nodes;
    QList<RowPiece> rowPieces = findRowPieces(nodes);

    if (!rowPieces.isEmpty() && isNodeConnectedToPowerSupply(rowPieces))
    {
        _resistanceValue = calculateResistanceValueFromRowPieces(rowPieces, nodes, mergeList);
        calculateVoltageAndAmp(rowPieces, mergeList);
        return _resistanceValue;
    }
    return 0.0;
}

QList<RowPiece> Calculator::findRowPieces(QList<Node*>& nodes)
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
            pathAnalysis(componentPort, analysisHasEndedSuccessful, &rowPieces, &nodes);
        }
    }
    return rowPieces;
}

void Calculator::findSameRowPieces(RowPiece rowpiece1, QList<RowPiece>& mergeList)
{
    for (RowPiece& rowpiece2 : mergeList)
    {
        if (rowpiece1.getComponents() == rowpiece2.getComponents())
        {
            rowpiece2.setAmp(rowpiece1.getAmp());
        }
    }
}

//Suche nach Nachbarn, die nicht direkt mit dem aktuellen ComponentPort verbunden sind
void Calculator::searchingForIndirectParallelNeighbours(QList<ComponentPort>& foundComponentPorts)
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

//Suche nach Nachbarn, die direkt mit dem aktuellen ComponentPort verbunden sind
void
Calculator::searchingForDirectParallelNeighbours(ComponentPort actualComPort,
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

QList<ComponentPort> Calculator::findFirstComponentPort(void)
{
    ComponentPort startOfSearch(nullptr, Component::Port::null, nullptr);
    for (Connection* connection : _connections)
    {
        if (Component::ComponentType::PowerSupply ==
            connection->getComponentPortOne().getComponent()->getComponentType())
        {
            startOfSearch = connection->getComponentPortOne();
            break;
        }
        else if (Component::ComponentType::PowerSupply ==
                 connection->getComponentPortTwo().getComponent()->getComponentType())
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

void Calculator::pathAnalysis(ComponentPort actualComponentPort, bool& hasAnalysisEndedSuccessful,
                              QList<RowPiece>* rowPieces, QList<Node*>* knownNodes)
{
    //Wenn der Knoten bereits bekannt ist, ist die Abbruchbedingung erreicht
    QList<Component*> rowPiecesComponents;
    rowPiecesComponents.append(actualComponentPort.getComponent());

    //Widerstand des RowPieces
    long double resistanceValueOfRowPiece = 0;

    /*
     * Finde alle ComponentPorts, die mit dem gegenüberliegenden Port von actualComponentPort verbunden sind.
     * So kann zwischen einer Reihen- und Parallelschaltung unterschieden werden.
     */
    QList<ComponentPort> neighbourComponentPortsOfStartNode = searchForNeighbours(actualComponentPort);
    Node* startNode = getOrCreateNode(actualComponentPort, neighbourComponentPortsOfStartNode, knownNodes);

    QList<ComponentPort> neighbourComponentPorts = searchForNeighbours(actualComponentPort.getOppisiteComponentPort());

    //der aktuelle ComponentPort muss gedreht werden, um den Endknoten bestimmen zu können
    actualComponentPort.invertPort();
    bool neighbourComponentPortsContainPowerSupply = isPowerSupplyinComponentPortList(neighbourComponentPorts);

    /*
     * Solange es eine Reihenschaltung ist und man nicht bei der Spannungsquelle wieder angekommen ist,
     * wird der Widerstandswert der Komponenten addiert
     */
    Resistor* resistor = dynamic_cast<Resistor*>(actualComponentPort.getComponent());
    bool isResistor = (nullptr != resistor);
    if (isResistor)
    {
        resistanceValueOfRowPiece += resistor->getResistanceValue();
    }
    else
    {
        return;
    }

    addingResistorsInRowToOneRowPiece(rowPiecesComponents, actualComponentPort, neighbourComponentPorts,
                                      neighbourComponentPortsContainPowerSupply, resistanceValueOfRowPiece);

    if (0 == neighbourComponentPorts.count())
    {
        //eine offene Verbindung wurde gefunden und hat den Wert ungültig gemacht
        hasAnalysisEndedSuccessful = false;
        return;
    }
    else
    {
        //finde oder erstelle den Knoten für das Ende
        Node* endNode = getOrCreateNode(actualComponentPort, neighbourComponentPorts, knownNodes);
        //erstelle das neue RowPiece
        RowPiece rowPiece(startNode, endNode, resistanceValueOfRowPiece, rowPiecesComponents);

        //Abbruchbedienung das gefundene RowPieces, darf nicht bekannt sein
        bool isAlreadyKnownRowPiece = rowPieces->contains(rowPiece);
        if (!isAlreadyKnownRowPiece)
        {
            rowPieces->append(rowPiece);

            //Überprüfung: ist eine Spannungsquelle unter den verbundenen Komponenten (Abbruchbedingung)
            bool neighbourComponentPortsContainPowerSupply = isPowerSupplyinComponentPortList(neighbourComponentPorts);

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

void Calculator::addingResistorsInRowToOneRowPiece(QList<Component*>& rowPiecesComponents,
                                                   ComponentPort& actualComponentPort,
                                                   QList<ComponentPort>& neighbourComponentPorts,
                                                   bool neighbourComponentPortsContainPowerSupply,
                                                   long double& resistanceValueOfRowPiece)
{
    while (neighbourComponentPorts.count() == 1 && !neighbourComponentPortsContainPowerSupply)
    {

        //der Widerstandswert des RowPieces wird addiert
        Resistor* resistor = dynamic_cast<Resistor*>(neighbourComponentPorts.last().getComponent());
        bool isResistor = (nullptr != resistor);
        if (isResistor)
        {
            resistanceValueOfRowPiece += resistor->getResistanceValue();
        }
        else
        {
            break;
        }
        //der nächste Widerstand wird betrachtet, damit Endknoten bestimmt werden kann
        actualComponentPort = neighbourComponentPorts.last().getOppisiteComponentPort();
        rowPiecesComponents.append(actualComponentPort.getComponent());
        //die mit dem nächsten Widerstand verbundenen Widerstände werden gesucht
        neighbourComponentPorts = searchForNeighbours(actualComponentPort);
        neighbourComponentPortsContainPowerSupply = isPowerSupplyinComponentPortList(neighbourComponentPorts);
    }
}

QList<Component*> Calculator::initializeAmpAndVoltage(QList<Component*> components)
{
    for (Component* c : components)
    {
        if (c->getComponentType() == Component::ComponentType::Resistor)
        {
            c->setAmp(0.0);
            c->setVoltage(0.0);
        }
    }
    return components;
}

void
Calculator::deltaToStar(QList<Node*>& nodes, bool& changedSomething, QList<RowPiece>& rowPieces, RowPiece& rowPieceA,
                        RowPiece& rowPieceB, RowPiece* searchedRowPieces)
{
    _hasUsedStarCalculation = true;
    //rp ist rowPieceC
    //Dreieck zu Stern
    QList<ComponentPort> componentPorts;
    Node* newNode = new Node(nodes.count(), componentPorts, false);
    nodes.append(newNode);

    //Erstellen der Stern-RowPieces und alte removen
    QList<RowPiece> listOfNewRowPieces = calculateStar(rowPieceA, rowPieceB,
                                                       *searchedRowPieces, newNode);
    rowPieces.removeOne(rowPieceA);
    rowPieces.removeOne(rowPieceB);
    rowPieces.removeOne(*searchedRowPieces);
    rowPieces.append(listOfNewRowPieces);
    changedSomething = true;
}

Node* Calculator::getOrCreateNode(ComponentPort componentPortForNewNode,
                                  QList<ComponentPort> connectedComponentPorts,
                                  QList<Node*>* knownNodes)
{
    for (Node* node : *knownNodes)
    {
        bool nodeIsKnown = node->getComponentPorts().contains(componentPortForNewNode);
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
    for (ComponentPort cp : connectedComponentPorts)
    {
        if (Component::ComponentType::PowerSupply == cp.getComponent()->getComponentType())
        {
            isConnectedToPowerSupply = true;
            break;
        }
    }
    Node* createdNode = new Node(knownNodes->count(), connectedComponentPorts, isConnectedToPowerSupply);
    knownNodes->append(createdNode);
    return createdNode;
}

QList<ComponentPort> Calculator::searchForNeighbours(ComponentPort componentPortForSearch)
{
    QList<ComponentPort> neighbours;
    searchingForDirectParallelNeighbours(componentPortForSearch, neighbours);
    return neighbours;
}

bool Calculator::isPowerSupplyinComponentPortList(const QList<ComponentPort> list)
{
    for (ComponentPort componentPort : list)
    {
        if (Component::ComponentType::PowerSupply == componentPort.getComponent()->getComponentType())
        {
            return true;
        }
    }
    return false;
}

bool Calculator::isNodeConnectedToPowerSupply(QList<RowPiece> rowPieces)
{
    QList<Node*> nodes;
    for (RowPiece rp : rowPieces)
    {
        if (rp.getNodeOne()->isConnectedToPowerSupply())
        {
            if (!nodes.contains(rp.getNodeOne()))
            {
                nodes.append(rp.getNodeOne());
            }
        }
        if (rp.getNodeTwo()->isConnectedToPowerSupply())
        {
            if (!nodes.contains(rp.getNodeTwo()))
            {
                nodes.append(rp.getNodeTwo());
            }
        }
    }
    return (nodes.count() == 2);
}

QList<RowPiece>
Calculator::calculateStar(RowPiece rowPieceA, RowPiece rowPieceB, RowPiece rowPieceC, Node* newNode)
{
    Node* equalNodeA = rowPieceA.getEqualNode(rowPieceB);
    Node* equalNodeB = rowPieceB.getEqualNode(rowPieceC);
    Node* equalNodeC = rowPieceC.getEqualNode(rowPieceA);

    //Summe aller Widerstände
    long double additionValue =
            rowPieceA.getResistanceValue() + rowPieceB.getResistanceValue() + rowPieceC.getResistanceValue();

    // Die drei Stern-Zweige
    long double starValueA = ((rowPieceA.getResistanceValue() * rowPieceB.getResistanceValue()) / additionValue);
    long double starValueB = ((rowPieceB.getResistanceValue() * rowPieceC.getResistanceValue()) / additionValue);
    long double starValueC = ((rowPieceA.getResistanceValue() * rowPieceC.getResistanceValue()) / additionValue);

    RowPiece rowPieceAB(newNode, equalNodeA, starValueA, rowPieceA.getComponents());
    RowPiece rowPieceBC(newNode, equalNodeB, starValueB, rowPieceB.getComponents());
    RowPiece rowPieceCA(equalNodeC, newNode, starValueC, rowPieceC.getComponents());

    QList<RowPiece> listOfNewRowPieces;
    listOfNewRowPieces.append(rowPieceAB);
    listOfNewRowPieces.append(rowPieceBC);
    listOfNewRowPieces.append(rowPieceCA);
    return listOfNewRowPieces;
}

void Calculator::calculateVoltageAndAmp(QList<RowPiece> rowpieces, QList<RowPiece>& mergeList)
{
    int ListSize = mergeList.count() - 1;
    long double totalCurrent = 0.0;
    if (mergeList.isEmpty())
    {
        for (Component* component : _components)
        {
            if (Component::PowerSupply == component->getComponentType())
            {
                component->setAmp(component->getVoltage() / _resistanceValue);
                totalCurrent = component->getAmp();
            }
        }
        for (RowPiece rowpiece : rowpieces)
        {
            for (Component* component : rowpiece.getComponents())
            {
                if (Component::Resistor == component->getComponentType())
                {
                    component->setAmp(totalCurrent);
                    Resistor* resistor = dynamic_cast<Resistor*>(component);
                    bool isResistor = (nullptr != resistor);
                    if (isResistor)
                    {
                        component->setVoltage(totalCurrent * resistor->getResistanceValue());
                    }
                }
            }
        }
    }
    else
    {
        for (Component* component : _components)
        {
            if (Component::PowerSupply == component->getComponentType())
            {
                component->setAmp(component->getVoltage() / _resistanceValue);
                mergeList[ListSize].setAmp(component->getAmp());
            }
        }
        for (int i = 0; i < (ListSize + 1) / 3; i++)
        {
            if (mergeList[ListSize - 3 * i].getIsMergedParallel())
            {
                RowPiece* mergedRowpieces = &mergeList[ListSize - 3 * i];
                RowPiece* rowpiece1 = &mergeList[ListSize - 3 * i - 1];
                RowPiece* rowpiece2 = &mergeList[ListSize - 3 * i - 2];

                rowpiece1->setAmp(mergedRowpieces->getAmp() * (rowpiece2->getResistanceValue() /
                                                               (rowpiece1->getResistanceValue() +
                                                                rowpiece2->getResistanceValue())));
                findSameRowPieces(*rowpiece1, mergeList);
                calculateVoltageAndAmpInResistor(rowpiece1);
                rowpiece2->setAmp(mergedRowpieces->getAmp() * (rowpiece1->getResistanceValue() /
                                                               (rowpiece1->getResistanceValue() +
                                                                rowpiece2->getResistanceValue())));
                findSameRowPieces(*rowpiece2, mergeList);
                calculateVoltageAndAmpInResistor(rowpiece2);
            }
            else
            {
                RowPiece* mergedRowpieces = &mergeList[ListSize - 3 * i];
                RowPiece* rowpiece1 = &mergeList[ListSize - 3 * i - 1];
                RowPiece* rowpiece2 = &mergeList[ListSize - 3 * i - 2];

                rowpiece1->setAmp(mergedRowpieces->getAmp());
                findSameRowPieces(*rowpiece1, mergeList);
                calculateVoltageAndAmpInResistor(rowpiece1);

                rowpiece2->setAmp(mergedRowpieces->getAmp());
                findSameRowPieces(*rowpiece2, mergeList);
                calculateVoltageAndAmpInResistor(rowpiece2);
            }
        }
    }
}

void Calculator::calculateVoltageAndAmpInResistor(RowPiece* rowpiece)
{
    for (Component* component : rowpiece->getComponents())
    {
        if (Component::Resistor == component->getComponentType())
        {
            component->setAmp(rowpiece->getAmp());
            Resistor* resistor = dynamic_cast<Resistor*>(component);
            bool isResistor = (nullptr != resistor);
            if (isResistor)
            {
                component->setVoltage(component->getAmp() * resistor->getResistanceValue());
            }
        }
    }
}

void Calculator::starMerge(bool& changedSomething, QList<RowPiece>& rowPieces, RowPiece& rowPieceA, RowPiece& rowPieceB,
                           Node* equalNode, QList<Node*> nodes)
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
    for (RowPiece rs : rowPieces)
    {
        if (rs.getNodeOne()->getId() == oppositeNode->getId() ||
            rs.getNodeTwo()->getId() == oppositeNode->getId())
        {
            count++;
        }
    }
    if (count >= 3)
    {
        for (ComponentPort cp :  oppositeNode->getComponentPorts())
        {
            for (RowPiece rs : rowPieces)
            {
                for (Component* c : rs.getComponents())
                {
                    if (c == cp.getComponent())
                    {
                        if (rs.getOppositeNode(oppositeNode) ==
                            rowPieceA.getOppositeNode(equalNode))
                        {
                            searchedRowPieces = new RowPiece(rs.getNodeOne(), rs.getNodeTwo(),
                                                             rs.getResistanceValue(),
                                                             rs.getComponents());
                        }
                        break;
                    }
                }
                if (found)
                { break; }
            }
            if (found)
            { break; }
        }

        if (oppositeNode != nullptr && searchedRowPieces != nullptr &&
            *searchedRowPieces != rowPieceA && *searchedRowPieces != rowPieceB)
        {
            deltaToStar(nodes, changedSomething, rowPieces, rowPieceA, rowPieceB, searchedRowPieces);
        }
    }
}

void
Calculator::paralleMerge(RowPiece& rowPieceA, RowPiece& rowPieceB, QList<RowPiece>& rowPieces, bool& changedSomething,
                         QList<RowPiece>& mergeList)
{
    mergeList.append(rowPieceA);
    mergeList.append(rowPieceB);
    rowPieceA.parallelMerge(rowPieceB);
    rowPieceA.setIsMergedParallel(true);
    mergeList.append(rowPieceA);
    rowPieceA.setIsMergedParallel(false);
    rowPieces.removeOne(rowPieceB);
    changedSomething = true;
}

void Calculator::rowMerge(RowPiece& rowPieceA, RowPiece& rowPieceB, QList<RowPiece>& rowPieces, bool& changedSomething,
                          QList<RowPiece>& mergeList)
{
    mergeList.append(rowPieceA);
    mergeList.append(rowPieceB);
    rowPieceA.rowMerge(rowPieceB);
    mergeList.append(rowPieceA);
    rowPieces.removeOne(rowPieceB);
    changedSomething = true;
}

long double Calculator::calculateResistanceValueFromRowPieces(QList<RowPiece> rowPieces, QList<Node*> nodes,
                                                              QList<RowPiece>& mergeList)
{
    while (1 < rowPieces.count())
    {
        bool changedSomething = doUsualReshaping(rowPieces, nodes, mergeList);
        if (!changedSomething)
        {
            doStarDeltaReshaping(rowPieces, nodes);
        }
    }
    return rowPieces.last().getResistanceValue();
}

int Calculator::countNodesInRowPieces(Node* nodeToCount, QList<RowPiece> listOfRowPieces)
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

bool Calculator::doUsualReshaping(QList<RowPiece>& rowPieces, QList<Node*>& nodes, QList<RowPiece>& mergeList)
{
    bool changedSomething = false;
    for (RowPiece& rowPieceA : rowPieces)
    {
        //Widerstand kurzgeschlossen
        if (rowPieceA.getNodeOne() == rowPieceA.getNodeTwo())
        {
            rowPieces.removeOne(rowPieceA);
            break;
        }
            //offenes Ende
        else if (rowPieceA.hasOpenEnd(nodes, rowPieces) &&
                 !(rowPieceA.getNodeOne()->isConnectedToPowerSupply() ||
                   rowPieceA.getNodeTwo()->isConnectedToPowerSupply()))
        {
            rowPieces.removeOne(rowPieceA);
            break;
        }

        for (RowPiece& rowPieceB : rowPieces)
        {
            //Parallelschaltung
            if (rowPieceA != rowPieceB && rowPieceA.hasEqualNodesOnBothSides(rowPieceB))
            {
                paralleMerge(rowPieceA, rowPieceB, rowPieces, changedSomething, mergeList);
                break;
            }
                //Reihenschaltung
            else if (rowPieceA != rowPieceB && rowPieceA.hasOneEqualNode(rowPieceB))
            {
                Node* equalNode = rowPieceA.getEqualNode(rowPieceB);
                if (equalNode != nullptr && !equalNode->isConnectedToPowerSupply())
                {
                    if (changedSomething)
                    {
                        break;
                    }

                    if (2 == countNodesInRowPieces(equalNode, rowPieces))
                    {
                        rowMerge(rowPieceA, rowPieceB, rowPieces, changedSomething, mergeList);
                        break;
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
    return changedSomething;
}

bool Calculator::doStarDeltaReshaping(QList<RowPiece>& rowPieces, QList<Node*>& nodes)
{
    bool changedSomething = false;
    for (RowPiece& rowPieceA : rowPieces)
    {
        for (RowPiece& rowPieceB : rowPieces)
        {
            if (rowPieceA != rowPieceB && rowPieceA.hasOneEqualNode(rowPieceB))
            {
                Node* equalNode = rowPieceA.getEqualNode(rowPieceB);
                if (equalNode != nullptr && !equalNode->isConnectedToPowerSupply())
                {
                    //Wenn die Anzahl der Node ComponentPort größer 3 ist, klingt es nach Stern-Dreieck, RowPieces werden nach Anbindung zum Node
                    //durchsucht
                    int count = 0;
                    for (RowPiece rs : rowPieces)
                    {
                        if (rs.getNodeOne()->getId() == equalNode->getId() ||
                            rs.getNodeTwo()->getId() == equalNode->getId())
                        {
                            count++;
                        }
                    }
                    if (count >= 3 && !changedSomething)
                    {
                        starMerge(changedSomething, rowPieces, rowPieceA, rowPieceB, equalNode, nodes);
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
    return changedSomething;
}
