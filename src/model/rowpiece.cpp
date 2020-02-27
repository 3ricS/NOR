//
// Created by erics on 024, 24, Februar.
//

#include "rowpiece.h"
#include "model/component.h"
#include "model/node.h"

RowPiece::RowPiece(Node* nodeOne, Node* nodeTwo, int resistanceValue, QList<Component*> includedComponents) : _nodeOne(
        nodeOne), _nodeTwo(nodeTwo),
                                                                                                              _resistanceValue(
                                                                                                                      resistanceValue),
                                                                                                              _components(
                                                                                                                      includedComponents)
{
}

bool RowPiece::operator==(const RowPiece& rhs)
{
    bool hasSameComponents = true;
    for (Component* component : _components)
    {
        if (!rhs._components.contains(component))
        {
            hasSameComponents = false;
            break;
        }
    }
    return hasSameComponents;
}

bool RowPiece::operator!=(const RowPiece& rhs)
{
    return !(operator==(rhs));
}

/*!
 * \brief Prüft ob ein RowPiece zu einem anderen RowPiece die gleichen Knoten an beiden Seiten besitzt.
 *
 * \param[in]    otherRowPiece
 *
 */
bool RowPiece::hasEqualNodesOnBothSides(RowPiece otherRowPiece)
{
    bool equalInDirection = (_nodeOne == otherRowPiece._nodeOne && _nodeTwo == otherRowPiece._nodeTwo);
    bool equalInOtherDirection = (_nodeTwo == otherRowPiece._nodeOne && _nodeOne == otherRowPiece._nodeTwo);
    if(otherRowPiece._components.last()->getName() == "R5" || _components.last()->getName() == "R5")
    {
        qDebug() << equalInDirection << equalInOtherDirection;
    }

    return equalInDirection || equalInOtherDirection;
}

/*!
 * \brief
 *
 * \param[in]    otherRowPiece
 *
 *
 */
void RowPiece::parallelMerge(RowPiece otherRowPiece)
{
    //Listen der Components vergleichen
    _components.append(otherRowPiece._components);

    //Werte zusammenrechnen
    double newResistanceValueCounter = _resistanceValue * otherRowPiece._resistanceValue;
    double newResistanceValueDenominator = _resistanceValue + otherRowPiece._resistanceValue;
    _resistanceValue = (newResistanceValueCounter / newResistanceValueDenominator);
}

void RowPiece::rowMerge(RowPiece otherRowPiece)
{
    //Listen der Components vergleichen
    _components.append(otherRowPiece._components);

    //Nodes zusammenführen
    bool nodeOneIsEqual = (otherRowPiece._nodeOne == _nodeOne || otherRowPiece._nodeTwo == _nodeOne);
    bool otherNodeOneIsEqual = (otherRowPiece._nodeOne == _nodeOne || otherRowPiece._nodeOne == _nodeTwo);
    if (otherNodeOneIsEqual)
    {
        if (nodeOneIsEqual)
        {
            _nodeOne = otherRowPiece._nodeTwo;
        }
        else
        {
            _nodeTwo = otherRowPiece._nodeTwo;
        }
        delete otherRowPiece._nodeOne;
    }
    else
    {
        if (nodeOneIsEqual)
        {
            _nodeOne = otherRowPiece._nodeOne;
        }
        else
        {
            _nodeTwo = otherRowPiece._nodeOne;
        }
        delete otherRowPiece._nodeTwo;
    }

    //Widerstandswerte addieren
    _resistanceValue = _resistanceValue + otherRowPiece._resistanceValue;
}

/*!
 * \brief Ob ein anderes RawPiece den gleichen Knoten besitzt.
 *
 * \param[in]    otherRowPiece
 * \return Ob der Knoten bei beiden RowPieces gleich ist.
 *
 */
bool RowPiece::hasOneEqualNode(RowPiece otherRowPiece)
{
    return nullptr != getEqualNode(otherRowPiece);
}

Node* RowPiece::getEqualNode(RowPiece otherRowPiece)
{
    bool nodeOneEqual = (_nodeOne == otherRowPiece._nodeOne || _nodeOne == otherRowPiece._nodeTwo);
    if (nodeOneEqual)
    {
        return _nodeOne;
    }
    bool nodeTwoEqual = (_nodeTwo == otherRowPiece._nodeOne || _nodeTwo == otherRowPiece._nodeTwo);
    if (nodeTwoEqual)
    {
        return _nodeTwo;
    }
    return nullptr;
}

/*!
 * \brief Prüft ob ein Knoten vorhanden ist.
 *
 * \param[in]    node
 * \return Ob ein Knoten vorhanden ist.
 *
 *
 */
bool RowPiece::hasNode(Node* node)
{
    return node == _nodeOne || node == _nodeTwo;
}
