/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Der Calculator berechnet den Gesamtwiderstandswert des Netztwerkes.
 *
 * Die Klasse ist ein Singleton.
 * Unter Verwendung der RowPieces berchnet der Calculator den Widerstandswert.
 * Die einzelnen RowPieces werden von innen nach ausßen zusammengefasst und deren Widerstandswerte zusammen gerechnet.
 */
#ifndef NOR_PUZZLECALCULATOR_H
#define NOR_PUZZLECALCULATOR_H

#include <QList>
#include <model/rowpiece.h>
#include <model/ComponentPort.h>

class Connection;
class Component;

class Calculator
{
public:
    static Calculator& calculator(void);
    long double calculate(QList<Connection*> connections, QList<Component*> components);

    //getter
    bool hasUsedStarCalculation(void) {return _hasUsedStarCalculation;}

private:
    //Singleton
    Calculator(void) = default;
    ~Calculator(void) = default;
    Calculator(const Calculator&) = delete;
    Calculator& operator=(const Calculator&) = delete;

    QList<RowPiece> findRowPieces(QList<Node *> &nodes);
    void findSameRowPieces(RowPiece rowpiece1, QList<RowPiece> &mergeList);
    long double calculateResistanceValueFromRowPieces(QList<RowPiece> rowPieces, QList<Node *> nodes, QList<RowPiece>& mergeList);
    void pathAnalysis(ComponentPort actualComponentPort, bool& hasAnalysisEndedSuccessful, QList<RowPiece>* rowPieces, QList<Node*>* knownNodes);
    void addingResistorsInRowToOneRowPiece(QList<Component*>& rowPiecesComponents, ComponentPort& actualComponentPort,
                                           QList<ComponentPort>& neighbourComponentPorts, bool neighbourComponentPortsContainPowerSupply,
                                           int& resistanceValueOfRowPiece);
    void triangleToStar(QList<Node*>& nodes, bool& changedSomething, QList<RowPiece>& rowPieces, RowPiece& rowPieceA, RowPiece& rowPieceB,
                        RowPiece* searchedRowPieces);

    //Suche nach Nachbarn, die nicht direkt mit dem aktuellen ComponentPort verbunden sind
    void searchingForIndirectParallelNeighbours(QList<ComponentPort> &foundComponentPorts);
    //Suche nach Nachbarn, die direkt mit dem aktuellen ComponentPort verbunden sind
    void searchingForDirectParallelNeighbours(ComponentPort actualComPort,
                                              QList<ComponentPort>& foundComponentPorts);
    QList<ComponentPort> searchForNeighbours(ComponentPort componentPortForSearch);

    QList<ComponentPort> findFirstComponentPort(void);
    Node* getOrCreateNode(ComponentPort componentPortForNewNode, QList<ComponentPort> connectedComponentPorts,
                          QList<Node*>* knownNodes);
    bool isPowerSupplyinComponentPortList(QList<ComponentPort> list);
    bool isNodeConnectedToPowerSupply(QList<RowPiece> rowPieces);

    int countNodesInRowPieces(Node* nodeToCount, QList<RowPiece> listOfRowPieces);
    QList<RowPiece> calculateStar(RowPiece rowPieceA, RowPiece rowPieceB, RowPiece rowPieceC, Node *newNode);

    void calculateVoltageAndAmp(QList<RowPiece> rowpieces, QList<RowPiece> &mergeList);
    void calculateVoltageAndAmpInResistor(RowPiece* rowpiece);
    void starMerge(bool& changedSomething, QList<RowPiece>& rowPieces, RowPiece& rowPieceA, RowPiece& rowPieceB, Node* equalNode, QList<Node*> nodes);
    void paralleMerge(RowPiece& rowPieceA, RowPiece& rowPieceB, QList<RowPiece>& rowPieces, bool& changedSomething, QList<RowPiece> &mergeList);
    void rowMerge(RowPiece& rowPieceA, RowPiece& rowPieceB, QList<RowPiece>& rowPieces, bool& changedSomething, QList<RowPiece> &mergeList);

    double _resistanceValue = 0;
    QList<Component*> _components;
    QList<Connection*> _connections;
    bool _hasUsedStarCalculation = false;

};


#endif //NOR_PUZZLECALCULATOR_H
