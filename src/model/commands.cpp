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
    _deletedConnections = _model->deleteComponentWithoutUndoAndGetDeletedConnections(_createdComponent);
}

void CommandAddComponent::redo()
{
    if (_createdComponent == nullptr)
    {
        _createdComponent = _model->createNewComponentWithoutUndo(_gridPosition, _componentType, _componentIsVertical);
    }
    else if (_createdComponent != nullptr)
    {
        _model->addComponentWithoutUndo(_createdComponent);
        for (Connection* connection : _deletedConnections)
        {
            _model->addConnectionWithoutUndo(connection);
        }
    }
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
    if (_createdConnection == nullptr)
    {
        _createdConnection = _model->addConnectionWithoutUndo(_componentPortA, _componentPortB);
    }
    else
    {
        _model->addConnectionWithoutUndo(_createdConnection);
    }
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
 * CommandEditComponnet
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
    if (_editedComponent != nullptr)
    {
        _editedComponent->setName(_oldName);
        _editedComponent->setValue(_oldValue);
        _editedComponent->setOrientation(_oldOrientation);
        _model->updateCalc();
    }
}

void CommandEditComponent::redo()
{
    if (nullptr != _editedComponent)
    {
        _editedComponent->setName(_newName);
        _editedComponent->setValue(_newValue);
        _editedComponent->setOrientation(_newOrientation);
        _model->updateCalc();
    }
}


/*
 * _______________________________________________________________________
 * CommandDeleteComponnet
 */
CommandDeleteComponent::CommandDeleteComponent(NetworkGraphics* model, Component* componentToDelete) :
        _model(model), _deletedComponent(componentToDelete)
{
}

void CommandDeleteComponent::undo()
{
    _model->addComponentWithoutUndo(_deletedComponent);
    for (Connection* connection : _deletedConnections)
    {
        _model->addConnectionWithoutUndo(connection);
    }
}

void CommandDeleteComponent::redo()
{
    _deletedConnections = _model->deleteComponentWithoutUndoAndGetDeletedConnections(_deletedComponent);
}

CommandDeleteComponent::~CommandDeleteComponent()
{
    for (Connection* connection : _deletedConnections)
    {
        delete connection;
    }
    delete _deletedComponent;
}
