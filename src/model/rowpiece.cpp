//
// Created by erics on 024, 24, Februar.
//

#include "rowpiece.h"
#include "model/component.h"
#include "model/node.h"

RowPiece::RowPiece(Node* nodeOne, Node* nodeTwo, int resistanceValue, QList<Component*> includedComponents) : _nodeOne(nodeOne), _nodeTwo(nodeTwo),
                                                                        _resistanceValue(resistanceValue), _components(includedComponents)
{
}

bool RowPiece::operator==(const RowPiece &rhs)
{
    bool hasSameComponents = true;
    for(Component* component : _components)
    {
        if(!rhs._components.contains(component))
        {
            hasSameComponents = false;
            break;
        }
    }
    return hasSameComponents;
}

bool RowPiece::operator!=(const RowPiece &rhs)
{
    return !(operator==(rhs));
}
