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
    _hasDoneUndo = true;
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
    _hasDoneUndo = false;
}

CommandAddComponent::~CommandAddComponent()
{
    if(_hasDoneUndo)
    {
        delete _createdComponent;
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
    _hasDoneUndo = true;
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
    _hasDoneUndo = false;
}

CommandAddConnection::~CommandAddConnection()
{
    if(_hasDoneUndo)
    {
        delete _createdConnection;
    }
}





/*
 * _______________________________________________________________________
 * CommandAddDescriptionField
 */
CommandAddDescriptionField::CommandAddDescriptionField(NetworkGraphics* model, DescriptionField* descriptionField) :
        _model(model), _createdDescriptionField(descriptionField)
{

}

void CommandAddDescriptionField::undo()
{
    _model->deleteDescriptionWithoutUndo(_createdDescriptionField);
    _hasDoneUndo = true;
}

void CommandAddDescriptionField::redo()
{
    _model->addDescriptionFieldWithoutUndo(_createdDescriptionField);
    _hasDoneUndo = false;
}

CommandAddDescriptionField::~CommandAddDescriptionField()
{
    if(_hasDoneUndo)
    {
        delete _createdDescriptionField;
    }
}



/*
 * _______________________________________________________________________
 * CommandMoveComponnet
 */

CommandMoveComponent::CommandMoveComponent(NetworkGraphics* model, Component* componentToMove,
                                           DescriptionField* descriptionToMove, QPointF gridPosition) :
        _model(model), _componentToMove(componentToMove), _descriptionToMove(descriptionToMove),
        _gridEndPosition(gridPosition)
{
    if(componentToMove != nullptr)
    {
        _gridComponentStartPosition = _componentToMove->getPosition();
    }
    else if(descriptionToMove != nullptr)
    {
        _gridDescriptionStartPosition = QPointF(descriptionToMove->getXPos(), descriptionToMove->getYPos());
    }
}

void CommandMoveComponent::undo()
{
    if(_componentToMove != nullptr)
    _model->moveComponentWithoutUndo(_componentToMove, _descriptionToMove, _gridComponentStartPosition);

    if(_descriptionToMove != nullptr)
    _model->moveComponentWithoutUndo(_componentToMove, _descriptionToMove, _gridDescriptionStartPosition);
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
    _hasDoneUndo = true;
}

void CommandDeleteComponent::redo()
{
    _deletedConnections = _model->deleteComponentWithoutUndoAndGetDeletedConnections(_deletedComponent);
    _hasDoneUndo = false;
}

CommandDeleteComponent::~CommandDeleteComponent()
{
    if(!_hasDoneUndo)
    {
        for (Connection* connection : _deletedConnections)
        {
            delete connection;
        }
        delete _deletedComponent;
    }
}




/*
 * _______________________________________________________________________
 * CommandDeleteComponnet
 */
CommandDeleteConnection::CommandDeleteConnection(NetworkGraphics* model, Connection* connectionToDelete) :
    _model(model), _deletedConnection(connectionToDelete)
{
}

void CommandDeleteConnection::undo()
{
    _hasDoneUndo = true;
    _model->addConnectionWithoutUndo(_deletedConnection);
}

void CommandDeleteConnection::redo()
{
    _hasDoneUndo = false;
    _model->deleteConnectionWithoutUndo(_deletedConnection);
}

CommandDeleteConnection::~CommandDeleteConnection()
{
    if(!_hasDoneUndo)
    {
        delete _deletedConnection;
    }
}







/*
 * _______________________________________________________________________
 * CommandDeleteDescription
 */

CommandDeleteDescription::CommandDeleteDescription(NetworkGraphics* model, DescriptionField* descriptionField) :
    _model(model), _deletedDescription(descriptionField)
{
}

CommandDeleteDescription::~CommandDeleteDescription()
{
    if(!_hasDoneUndo)
    {
        delete _deletedDescription;
    }
}

void CommandDeleteDescription::undo()
{
    _hasDoneUndo = true;
    _model->addDescriptionFieldWithoutUndo(_deletedDescription);
}

void CommandDeleteDescription::redo()
{
    _hasDoneUndo = false;
    _model->deleteDescriptionWithoutUndo(_deletedDescription);
}

CommandDuplicateComponent::CommandDuplicateComponent(NetworkGraphics *model, Component *componentToDuplicate, int xPosition, int yPosition) :
    _model(model), _componentToDuplicate(componentToDuplicate), _xPosition(xPosition), _yPosition(yPosition)
{

}

void CommandDuplicateComponent::undo()
{
    if(_createdComponent != nullptr)
    {
        _model->deleteComponentWithoutUndoAndGetDeletedConnections(_createdComponent);
    }
}

void CommandDuplicateComponent::redo()
{
    if(_componentToDuplicate != nullptr)
    {
        _createdComponent = _model->duplicateComponentWithoutUndo(_componentToDuplicate, _xPosition, _yPosition);
    }
}
