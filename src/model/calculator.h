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

#include <model/componentport.h>
#include <model/rowpiece.h>

class Component;
class Connection;

class Calculator
{
public:
    static Calculator& calculator(void);
    long double calculate(QList<Connection*> connections, QList<Component*> components);

    //getter
    /*!
     * \brief Liefert true, wenn eine Stern-Dreieck-Umformung durchgeführt wurde
     */
    bool hasUsedStarCalculation(void) {return _hasUsedStarCalculation;}

private:
    //Singleton
    Calculator(void) = default;
    ~Calculator(void) = default;
    Calculator(const Calculator&) = delete;
    Calculator& operator=(const Calculator&) = delete;


    //search
    void pathAnalysis(ComponentPort actualComponentPort, bool& hasAnalysisEndedSuccessful, QList<RowPiece>* rowPieces, QList<Node*>* knownNodes);
    QList<RowPiece> findRowPieces(QList<Node *> &nodes);
    QList<ComponentPort> findFirstComponentPort(void);
    void findSameRowPieces(RowPiece rowpiece1, QList<RowPiece> &mergeList);
    bool isPowerSupplyinComponentPortList(QList<ComponentPort> list);
    bool isNodeConnectedToPowerSupply(QList<RowPiece> rowPieces);
    int countNodesInRowPieces(Node* nodeToCount, QList<RowPiece> listOfRowPieces);
    Node* getOrCreateNode(ComponentPort componentPortForNewNode, QList<ComponentPort> connectedComponentPorts,
                          QList<Node*>* knownNodes);

    void searchingForIndirectParallelNeighbours(QList<ComponentPort> &foundComponentPorts);
    void searchingForDirectParallelNeighbours(ComponentPort actualComPort,
                                              QList<ComponentPort>& foundComponentPorts);
    QList<ComponentPort> searchForNeighbours(ComponentPort componentPortForSearch);

    void addingResistorsInRowToOneRowPiece(QList<Component*>& rowPiecesComponents, ComponentPort& actualComponentPort,
                                           QList<ComponentPort>& neighbourComponentPorts, bool neighbourComponentPortsContainPowerSupply,
                                           int& resistanceValueOfRowPiece);
    //preparation
    QList<Component*> initializeAmpAndVoltage(QList<Component*> components);

    //evaluate
    long double calculateResistanceValueFromRowPieces(QList<RowPiece> rowPieces, QList<Node *> nodes, QList<RowPiece>& mergeList);
    bool doUsualReshaping(QList<RowPiece>& rowPieces, QList<Node*>& nodes, QList<RowPiece>& mergeList);
    bool doStarDeltaReshaping(QList<RowPiece>& rowPieces, QList<Node*>& nodes);
    QList<RowPiece> calculateStar(RowPiece rowPieceA, RowPiece rowPieceB, RowPiece rowPieceC, Node *newNode);
    void deltaToStar(QList<Node*>& nodes, bool& changedSomething, QList<RowPiece>& rowPieces, RowPiece& rowPieceA, RowPiece& rowPieceB,
                     RowPiece* searchedRowPieces);

    void calculateVoltageAndAmp(QList<RowPiece> rowpieces, QList<RowPiece> &mergeList);
    void calculateVoltageAndAmpInResistor(RowPiece* rowpiece);

    void starMerge(bool& changedSomething, QList<RowPiece>& rowPieces, RowPiece& rowPieceA, RowPiece& rowPieceB, Node* equalNode, QList<Node*> nodes);
    void paralleMerge(RowPiece& rowPieceA, RowPiece& rowPieceB, QList<RowPiece>& rowPieces, bool& changedSomething, QList<RowPiece> &mergeList);
    void rowMerge(RowPiece& rowPieceA, RowPiece& rowPieceB, QList<RowPiece>& rowPieces, bool& changedSomething, QList<RowPiece> &mergeList);

    long double _resistanceValue = 0;
    QList<Component*> _components;
    QList<Connection*> _connections;
    bool _hasUsedStarCalculation = false;

};


#endif //NOR_PUZZLECALCULATOR_H
