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


/*
 * _______________________________________________________________________
 * CommandMoveComponnet
 */

CommandMoveComponent::CommandMoveComponent(NetworkGraphics* model, Component* componentToMove,
                                           DescriptionField* descriptionToMove, QPointF gridPosition) :
        _model(model), _componentToMove(componentToMove), _descriptionToMove(descriptionToMove),
        _gridEndPosition(gridPosition), _gridStartPosition(componentToMove->getPosition())
{
}

void CommandMoveComponent::undo()
{
    _model->moveComponentWithoutUndo(_componentToMove, _descriptionToMove, _gridStartPosition);
}

void CommandMoveComponent::redo()
{
    _model->moveComponentWithoutUndo(_componentToMove, _descriptionToMove, _gridEndPosition);
}


/*
 * _______________________________________________________________________
 * CommandMoveComponnet
 */
CommandEditComponent::CommandEditComponent(NetworkGraphics* model, Component* editedComponent,
                                           Component::Orientation originalOrientation, QString newName,
                                           double newValue) :
        _model(model), _editedComponent(editedComponent), _oldOrientation(originalOrientation),
        _newName(newName), _newValue(newValue)
{
    _oldName = editedComponent->getName();
    _oldValue = editedComponent->getValue();
    _newOrientation = editedComponent->getOrientation();
}

void CommandEditComponent::undo()
{
    if(_editedComponent != nullptr)
    {
        _editedComponent->setName(_oldName);
        _editedComponent->setValue(_oldValue);
        _editedComponent->setOrientation(_oldOrientation);
        _model->updateCalc();
    }
}

void CommandEditComponent::redo()
{
    if(nullptr != _editedComponent)
    {
        _editedComponent->setName(_newName);
        _editedComponent->setValue(_newValue);
        _editedComponent->setOrientation(_newOrientation);
        _model->updateCalc();
    }
}

