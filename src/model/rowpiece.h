//
// Created by erics on 024, 24, Februar.
//

#ifndef NOR_ROWPIECE_H
#define NOR_ROWPIECE_H

#include <QList>

class Component;
class Node;

class RowPiece
{
public:
    RowPiece(Node* nodeOne, Node* nodeTwo, int resistanceValue);
    int getResistanceValue(void) {return _resistanceValue;}

private:
    int _resistanceValue;
    Node* _nodeOne;
    Node* _nodeTwo;
};


#endif //NOR_ROWPIECE_H
