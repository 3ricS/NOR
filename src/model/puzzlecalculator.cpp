//
// Created by erics on 024, 24, Februar.
//

#include "puzzlecalculator.h"
#include "model/node.h"
#include "model/connection.h"
#include "model/component.h"

PuzzleCalculator::PuzzleCalculator(QList<Connection*> connections, QList<Component*> components) : _connections(
        connections), _components(components)
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
    ComponentPort startOfSearch = findFirstComponentPort();
    bool foundPowerSupply = (nullptr != startOfSearch.getComponent());

    if(foundPowerSupply)
    {
        QList<RowPiece> rowPieces;
        QList<Node*> nodes;

        //Beginn der rekursiven Suche nach RowPieces
        bool analysisHasEndedSuccessful = true;
        pathAnalysis(startOfSearch, analysisHasEndedSuccessful, &rowPieces, &nodes);

        if (analysisHasEndedSuccessful)
        {
            //test
            for (RowPiece rp : rowPieces)
            {
                qDebug() << rp.getResistanceValue();
            }
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
            if (cp == con->getComponentPortOne() && !newFound.contains(con->getComponentPortTwo()))
            {
                newFound.append(con->getComponentPortTwo());
            }
            else if (cp == con->getComponentPortTwo() && !newFound.contains(con->getComponentPortOne()))
            {
                newFound.append(con->getComponentPortOne());
            }
        }
    }

    if (newFound.count() != 0)
    {
        //nur die neu gefundenen Komponenten werden auf Nachbarn untersucht
        searchingForIndirectParallelNeighbours(newFound);

        for (ComponentPort cp : newFound)
        {
            foundComponentPorts.append(cp);
        }
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

        searchingForIndirectParallelNeighbours(foundComponentPorts);
    }
}

ComponentPort PuzzleCalculator::findFirstComponentPort()
{
    ComponentPort startOfSearch(nullptr, Component::Port::null);
    for (Connection* connection : _connections)
    {
        if (Component::ComponentType::PowerSupply ==
            connection->getComponentPortOne().getComponent()->getComponentType())
        {
            startOfSearch = connection->getComponentPortTwo();
            break;
        }
        else if (Component::ComponentType::PowerSupply ==
                 connection->getComponentPortTwo().getComponent()->getComponentType())
        {
            startOfSearch = connection->getComponentPortOne();
            break;
        }
    }
    bool foundStartOfSearch = (startOfSearch.getComponent() != nullptr);
    if (foundStartOfSearch)
    {
        qDebug() << "PuzzleCalculator.cpp: PowerSupply nicht verbunden";
        return startOfSearch;
    }
    return startOfSearch;
}

void PuzzleCalculator::pathAnalysis(ComponentPort actualComponentPort, bool& hasAnalysisEndedSuccessful,
                                    QList<RowPiece>* rowPieces, QList<Node*>* knownNodes)
{
    //Wenn der Knoten bereits bekannt ist, ist die Abbruchbedingung erreicht
    bool nodeIsKnown = true;
    Node* startNode = getOrCeateNode(actualComponentPort, QList<ComponentPort>(), nodeIsKnown, knownNodes);
    if (!nodeIsKnown)
    {
        int resistanceValueOfRowPiece = 0;
        /*
         * Finde alle ComponentPorts, die mit dem gegenüberliegenden Port von actualComponentPort verbunden sind.
         * So kann zwischen einer Reihen- und Parallelschaltung unterschieden werden.
         */
        QList<ComponentPort> neighbourComponentPorts = searchForNeighbours(
                actualComponentPort.getOppisiteComponentPort());
        //der aktuelle ComponentPort muss gedreht werden, um den Endknoten bestimmen zu können
        actualComponentPort.invertPort();

        bool neighbourComponentPortsContainPowerSupply = false;
        /*
         * Solange es eine Reihenschaltung ist und man nicht bei der Spannungsquelle wieder angekommen ist,
         * wird der Widerstandswert der Komponenten addiert
         */
        while (neighbourComponentPorts.count() == 1 && !neighbourComponentPortsContainPowerSupply)
        {
            //der Widerstandswert des RowPieces wird addiert
            resistanceValueOfRowPiece += neighbourComponentPorts.last().getComponent()->getValue();
            //der nächste Widerstand wird betrachtet, damit Endknoten bestimmt werden kann
            actualComponentPort = neighbourComponentPorts.last().getOppisiteComponentPort();
            //die mit dem nächsten Widerstand verbundenen Widerstände werden gesucht
            neighbourComponentPorts = searchForNeighbours(actualComponentPort);

            //Überprüfung: ist eine Spannungsquelle unter den verbundenen Komponenten (Abbruchbedingung)
            neighbourComponentPortsContainPowerSupply = isPowerSupplyinComponentPortList(neighbourComponentPorts);
        }

        if (neighbourComponentPortsContainPowerSupply)
        {
            //PowerSupply wiedergefunden
            return;
        }
        else if (0 == neighbourComponentPorts.count())
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
            RowPiece rowPiece(startNode, endNode, resistanceValueOfRowPiece);
            //TODO: ist hier ein contains nötig?
            rowPieces->append(rowPiece);

            bool neighbourComponentPortsContainPowerSupply = isPowerSupplyinComponentPortList(neighbourComponentPorts);
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
}
