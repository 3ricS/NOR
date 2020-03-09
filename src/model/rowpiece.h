/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein RowPiece ist ein Objekt, welches aus Widerständen zusammengesetzt ist, um den Gesamtwiderstand zu berechnen.
 */

#ifndef NOR_ROWPIECE_H
#define NOR_ROWPIECE_H

#include <QList>
#include <model/component.h>

class Node;

class RowPiece
{
public:
    RowPiece(Node* nodeOne, Node* nodeTwo, double resistanceValue, QList<Component *> includedComponents);

    void parallelMerge(RowPiece otherRowPiece);
    void rowMerge(RowPiece otherRowPiece);

    double getResistanceValue(void) {return _resistanceValue;}

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
    double _resistanceValue;
    double _amp = 0.0;
    Node* _nodeOne;
    Node* _nodeTwo;
    QList<Component*> _components;
    bool _isMergedParallel = false;
};


#endif //NOR_ROWPIECE_H
