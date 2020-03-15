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

    void parallelMerge(RowPiece otherRowPiece);
    void rowMerge(RowPiece otherRowPiece);

    long double getResistanceValue(void) {return _resistanceValue;}

    Node* getNodeOne(void) {return _nodeOne;}
    Node* getNodeTwo(void) {return _nodeTwo;}

    bool getIsMergedParallel() {return _isMergedParallel;}
    void setIsMergedParallel(bool isMergedParallel) {_isMergedParallel = isMergedParallel;}

    double getAmp(void) {return _amp;}
    void setAmp(double amp) {_amp = amp;}

    bool operator==(const RowPiece& rhs);
    bool operator!=(const RowPiece& rhs);

    bool hasEqualNodesOnBothSides(RowPiece otherRowPiece);
    bool hasOneEqualNode(RowPiece otherRowPiece);

    QList<Component*> getComponents(void) {return _components;}
    Node* getEqualNode(RowPiece otherRowPiece);
    Node* getOppositeNode(Node* node);

    bool hasNode(Node* node);

private:
    long double _resistanceValue;
    double _amp = 0.0;
    Node* _nodeOne;
    Node* _nodeTwo;
    QList<Component*> _components;
    bool _isMergedParallel = false;
};


#endif //NOR_ROWPIECE_H
