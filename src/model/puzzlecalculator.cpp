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
 * \param[in]   connections
 * \param[in]   components
 * \return Gibt den Widerstandswert als double zurück.
 *
 *
 */
double PuzzleCalculator::calculate(QList<Connection*> connections, QList<Component*> components)
{
    _connections = connections;
    _components = components;

    QList<RowPiece> rowPieces = findRowPieces();
    if (!rowPieces.isEmpty())
    {
        double resistanceValue = calculateResistanceValueFromRowPieces(rowPieces);
        return resistanceValue;
    }
    return 0.0;
}

void PuzzleCalculator::setLists(QList<Connection*> connections, QList<Component*> components)
{
    _connections = connections;
    _components = components;
}

QList<RowPiece> PuzzleCalculator::findRowPieces()
{
    //Anfang suchen
    QList<ComponentPort> startOfSearchComponentsPorts = findFirstComponentPort();
    bool foundPowerSupply = (!startOfSearchComponentsPorts.isEmpty());

    QList<RowPiece> rowPieces;
    if (foundPowerSupply)
    {
        QList<Node*> nodes;

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

QList<ComponentPort> PuzzleCalculator::findFirstComponentPort()
{
    ComponentPort startOfSearch(nullptr, Component::Port::null);
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
    Node* startNode = getOrCeateNode(actualComponentPort, neighbourComponentPortsOfStartNode, nodeIsKnown, knownNodes);

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
        Node* endNode = getOrCeateNode(actualComponentPort, neighbourComponentPorts, nodeIsKnown, knownNodes);
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

Node* PuzzleCalculator::getOrCeateNode(ComponentPort componentPortForNewNode,
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
        if(Component::ComponentType::PowerSupply == cp.getComponent()->getComponentType())
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
        if (Component::ComponentType::PowerSupply == componentPort.getComponent()->getComponentType())
        {
            return true;
        }
    }
    return false;
}

double PuzzleCalculator::calculateResistanceValueFromRowPieces(QList<RowPiece> rowPieces)
{
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
                    rowPieceA.parallelMerge(rowPieceB);
                    rowPieces.removeOne(rowPieceB);
                    changedSomething = true;
                    break;
                }
                else if (rowPieceA != rowPieceB && rowPieceA.hasOneEqualNode(rowPieceB))
                {
                    Node* equalNode = rowPieceA.getEqualNode(rowPieceB);
                    if (equalNode != nullptr && !equalNode->isConnectedToPowerSupply())
                    {
                        if (2 == countNodesInRowPieces(equalNode, rowPieces))
                        {
                            rowPieceA.rowMerge(rowPieceB);
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
            }
            if (changedSomething)
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
