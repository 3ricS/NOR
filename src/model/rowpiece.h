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
    int getResistanceValue(void) {return _resistanceValue;}

    bool operator==(const RowPiece& rhs);
    bool operator!=(const RowPiece& rhs);

private:
    int _resistanceValue;
    Node* _nodeOne;
    Node* _nodeTwo;
    QList<Component*> _components;
};


#endif //NOR_ROWPIECE_H
