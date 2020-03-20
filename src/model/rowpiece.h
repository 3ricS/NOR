/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein RowPiece ist ein Objekt, welches aus zusammengesetzten Widerständen besteht, um den Gesamtwiderstand zu berechnen.
 *
 * RowPieces sind zusammengeführte Widerstände die parallel oder in reihe zueinander liegen.
 * Ein Rowpiece besitzt zwei Knoten, einen Widerstandswert und einer Liste von Komponenten aus der das RowPiece besteht.
 * Jedes RowPiece besitzt an jedem ende einen Knoten.
 * Wenn sich mehrere Rowpieces in reihe zueinander befinden werden diese zusammengefasst.
 * Entsprechend passiert dies, wenn sich die RowPieces parallel zueinander befinden.
 */
#ifndef NOR_ROWPIECE_H
#define NOR_ROWPIECE_H

#include <QList>
#include <model/component.h>

class Node;

class RowPiece
{
public:
    RowPiece(Node* nodeOne, Node* nodeTwo, long double resistanceValue, QList<Component *> includedComponents);
    bool operator==(const RowPiece& rhs);
    bool operator!=(const RowPiece& rhs);

    void parallelMerge(RowPiece otherRowPiece);
    void rowMerge(RowPiece otherRowPiece);

    bool hasEqualNodesOnBothSides(RowPiece otherRowPiece);
    bool hasOneEqualNode(RowPiece otherRowPiece);
    bool hasNode(Node* node);
    bool hasOpenEnd(QList<Node*> allNodes, QList<RowPiece> rowPieces);

    //getter
    long double getResistanceValue(void) {return _resistanceValue;}
    Node* getNodeOne(void) {return _nodeOne;}
    Node* getNodeTwo(void) {return _nodeTwo;}
    bool getIsMergedParallel(void) {return _isMergedParallel;}
    void setIsMergedParallel(bool isMergedParallel) {_isMergedParallel = isMergedParallel;}
    double getAmp(void) {return _amp;}
    QList<Component*> getComponents(void) {return _components;}
    Node* getEqualNode(RowPiece otherRowPiece);
    Node* getOppositeNode(Node* node);

    //setter
    void setAmp(double amp) {_amp = amp;}

private:
    QList<Node*> getConnectedNodes(QList<Node*> allNodes);
    int countConnectedRowPiecesOfNode(Node* node, QList<RowPiece> rowPieces);

    Node* _nodeOne;
    Node* _nodeTwo;
    long double _resistanceValue;
    double _amp = 0.0;
    QList<Component*> _components;
    bool _isMergedParallel = false;
};


#endif //NOR_ROWPIECE_H
