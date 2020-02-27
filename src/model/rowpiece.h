//
// Created by erics on 024, 24, Februar.
//

#ifndef NOR_ROWPIECE_H
#define NOR_ROWPIECE_H

#include <QList>
#include <model/component.h>

class Node;

class RowPiece
{
public:
    RowPiece(Node* nodeOne, Node* nodeTwo, int resistanceValue, QList<Component *> includedComponents);

    void parallelMerge(RowPiece otherRowPiece);
    void rowMerge(RowPiece otherRowPiece);

    double getResistanceValue(void) {return _resistanceValue;}

    Node* getNodeOne() {return _nodeOne;}
    Node* getNodeTwo() {return _nodeTwo;}

    bool operator==(const RowPiece& rhs);
    bool operator!=(const RowPiece& rhs);

    bool hasEqualNodesOnBothSides(RowPiece otherRowPiece);
    bool hasOneEqualNode(RowPiece otherRowPiece);

    QList<Component*> getComponents() {return _components;}
    Node* getEqualNode(RowPiece otherRowPiece);

    bool hasNode(Node* node);

private:
    double _resistanceValue;
    Node* _nodeOne;
    Node* _nodeTwo;
    QList<Component*> _components;
};


#endif //NOR_ROWPIECE_H
