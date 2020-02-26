//
// Created by erics on 026, 26, Februar.
//

#include "commands.h"
#include <model/networkgraphics.h>

CommandAddComponent::CommandAddComponent(NetworkGraphics* model, QPointF gridPosition,
                                         Component::ComponentType componentType,
                                         bool componentIsVertical) :
        _model(model), _gridPosition(gridPosition), _componentType(componentType),
        _componentIsVertical(componentIsVertical)
{

}

void CommandAddComponent::undo()
{
    _model->deleteComponentWithoutUndo(_createdComponent);
}

void CommandAddComponent::redo()
{
    _createdComponent = _model->createNewComponentWithoutUndo(_gridPosition, _componentType, _componentIsVertical);
}




/*
 * _______________________________________________________________________
 * CommandAddConnection
 */

CommandAddConnection::CommandAddConnection(NetworkGraphics* model, ComponentPort componentPortA,
                                           ComponentPort componentPortB) :
        _model(model), _componentPortA(componentPortA), _componentPortB(componentPortB)
{
}

void CommandAddConnection::undo()
{
    _model->deleteConnectionWithoutUndo(_createdConnection);
}

void CommandAddConnection::redo()
{
    _createdConnection = _model->addConnectionWithoutUndo(_componentPortA, _componentPortB);
}
