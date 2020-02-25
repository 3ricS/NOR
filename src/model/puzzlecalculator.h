//
// Created by erics on 024, 24, Februar.
//

#ifndef NOR_PUZZLECALCULATOR_H
#define NOR_PUZZLECALCULATOR_H

#include <QList>
#include <model/rowpiece.h>
#include <model/componentport.h>

class Connection;
class Component;

class PuzzleCalculator
{
public:
    PuzzleCalculator(QList<Connection*> connections, QList<Component*> components);

    void calculate();

    //setter
    void setLists(QList<Connection*> connections, QList<Component*> components);

private:
    QList<RowPiece> findRowPieces();
    void pathAnalysis(ComponentPort actualComponentPort, bool& hasAnalysisEndedSuccessful,
                      QList<RowPiece>* rowPieces, QList<Node*>* knownNodes);

    //Suche nach Nachbarn, die nicht direkt mit dem aktuellen ComponentPort verbunden sind
    void searchingForIndirectParallelNeighbours(QList<ComponentPort> &foundComponentPorts);
    //Suche nach Nachbarn, die direkt mit dem aktuellen ComponentPort verbunden sind
    void searchingForDirectParallelNeighbours(ComponentPort actualComPort,
                                              QList<ComponentPort>& foundComponentPorts);
    QList<ComponentPort> searchForNeighbours(ComponentPort componentPortForSearch);

    QList<ComponentPort> findFirstComponentPort();
    Node* getOrCeateNode(ComponentPort componentPortForNewNode, QList<ComponentPort> connectedComponentPorts,
                         bool& nodeIsKnown, QList<Node*>* knownNodes);
    bool isPowerSupplyinComponentPortList(QList<ComponentPort> list);


    QList<Component*> _components;
    QList<Connection*> _connections;
};


#endif //NOR_PUZZLECALCULATOR_H
