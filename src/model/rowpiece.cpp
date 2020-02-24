//
// Created by erics on 024, 24, Februar.
//

#include "rowpiece.h"
#include "model/component.h"
#include "model/node.h"

RowPiece::RowPiece(Node* nodeOne, Node* nodeTwo, int resistanceValue) : _nodeOne(nodeOne), _nodeTwo(nodeTwo),
                                                                        _resistanceValue(resistanceValue)
{
}
