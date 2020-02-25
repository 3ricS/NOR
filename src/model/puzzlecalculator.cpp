//
// Created by erics on 024, 24, Februar.
//

#include "puzzlecalculator.h"
#include "model/node.h"
#include "model/connection.h"
#include "model/component.h"

PuzzleCalculator::PuzzleCalculator(QList<Connection*> connections, QList<Component*> components) :  _components(components),
    _connections(connections)
{

}

void PuzzleCalculator::calculate()
{
    QList<RowPiece> rowPieces = findRowPieces();
    //TODO: calculate
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
    if(foundPowerSupply)
    {
        QList<Node*> nodes;

        //Beginn der rekursiven Suche nach RowPieces
        bool analysisHasEndedSuccessful = true;
        for(ComponentPort componentPort : startOfSearchComponentsPorts)
        {
            qDebug() << "BeginnComponent" << componentPort.getComponent()->getName() << "Port" << componentPort.getPort();
            pathAnalysis(componentPort, analysisHasEndedSuccessful, &rowPieces, &nodes);
        }

        if (analysisHasEndedSuccessful)
        {
            //test
            for (RowPiece rp : rowPieces)
            {
                qDebug() << "Widerstand RowPiece" <<rp.getResistanceValue();
            }
            for(Node* n : nodes)
            {
                QString comps = "";
                for(ComponentPort cp : n->getComponentPorts())
                {
                    comps += (cp.getComponent()->getName() + " ");
                }
                qDebug() << "Node " <<
                            comps;
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
    QList<ComponentPort> neighbourComponentPorts = searchForNeighbours(
                actualComponentPort.getOppisiteComponentPort());
    Node* startNode = getOrCeateNode(actualComponentPort, neighbourComponentPorts, nodeIsKnown, knownNodes);
    //der aktuelle ComponentPort muss gedreht werden, um den Endknoten bestimmen zu können
    actualComponentPort.invertPort();
    bool neighbourComponentPortsContainPowerSupply = false;

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
        rowPiecesComponents.append(actualComponentPort.getComponent());
        //die mit dem nächsten Widerstand verbundenen Widerstände werden gesucht
        neighbourComponentPorts = searchForNeighbours(actualComponentPort);
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
        if(isAlreadyKnownRowPiece)
        {
            qDebug() << "Abbruchbedingus" << actualComponentPort.getComponent()->getName();
        }
        if(!isAlreadyKnownRowPiece)
        {
            rowPieces->append(rowPiece);

            //Überprüfung: ist eine Spannungsquelle unter den verbundenen Komponenten (Abbruchbedingung)
            bool neighbourComponentPortsContainPowerSupply = isPowerSupplyinComponentPortList(neighbourComponentPorts);
            qDebug() << "found power supply " << neighbourComponentPortsContainPowerSupply << neighbourComponentPorts.count();

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
        bool thisNodeIsKnown = node->getComponentPorts().contains(componentPortForNewNode);
        if (thisNodeIsKnown)
        {
            nodeIsKnown = true;
            return node;
        }
        else
        {
            nodeIsKnown = false;
        }
    }

    /*
     * componentPortForNewNode ist mit keinem bekannten Node verbunden.
     * Daher wird ein neuer Node erstellt
     */
    connectedComponentPorts.append(componentPortForNewNode);
    Node* createdNode = new Node(knownNodes->count(), connectedComponentPorts);
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
