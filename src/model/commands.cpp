#include "model/commands.h"
#include <model/networkgraphics.h>

CommandAddComponent::CommandAddComponent(NetworkGraphics* model, QPointF gridPosition,
                                         Component::ComponentType componentType,
                                         bool componentIsVertical) :
        _model(model), _gridPosition(gridPosition), _componentType(componentType),
        _componentIsVertical(componentIsVertical)
{
}

/*!
 * \brief Undo des Hinzufügen einer Komponente.
 */
void CommandAddComponent::undo(void)
{
    _deletedConnections = _model->deleteComponentWithoutUndoAndGetDeletedConnections(_createdComponent);
    _hasDoneUndo = true;
}

/*!
 * \brief Redo der zuvor entfernten Komponente.
 */
void CommandAddComponent::redo(void)
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

CommandAddComponent::~CommandAddComponent(void)
{
    if (_hasDoneUndo)
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

/*!
 * \brief Undo des Hinzufügen einer Connection.
 */
void CommandAddConnection::undo(void)
{
    _model->deleteConnectionWithoutUndo(_createdConnection);
    _hasDoneUndo = true;
}

/*!
 * \brief Redo der zuvor enfernten Connection.
 */
void CommandAddConnection::redo(void)
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

CommandAddConnection::~CommandAddConnection(void)
{
    if (_hasDoneUndo)
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

/*!
 * \brief Undo des Hinuufügen eines Textfeldes.
 */
void CommandAddDescriptionField::undo(void)
{
    _model->deleteDescriptionWithoutUndo(_createdDescriptionField);
    _hasDoneUndo = true;
}

/*!
 * \brief Redo des zuvor entfernten Textfeldes.
 */
void CommandAddDescriptionField::redo(void)
{
    _model->addDescriptionFieldWithoutUndo(_createdDescriptionField);
    _hasDoneUndo = false;
}

CommandAddDescriptionField::~CommandAddDescriptionField(void)
{
    if (_hasDoneUndo)
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
    if (componentToMove != nullptr)
    {
        _gridComponentStartPosition = _componentToMove->getPosition();
    }
    else if (descriptionToMove != nullptr)
    {
        _gridDescriptionStartPosition = QPointF(descriptionToMove->getXPosition(), descriptionToMove->getYPosition());
    }
}

/*!
 * \brief Undo des verschieben einer Komponente zurück zur vorherigen Position.
 */
void CommandMoveComponent::undo(void)
{
    if (_componentToMove != nullptr)
    {
        _model->moveComponentWithoutUndo(_componentToMove, _descriptionToMove, _gridComponentStartPosition);
    }

    if (_descriptionToMove != nullptr)
    {
        _model->moveComponentWithoutUndo(_componentToMove, _descriptionToMove, _gridDescriptionStartPosition);
    }
}

/*!
 * \brief Komponente an die vorherige Position verschieben.
 */
void CommandMoveComponent::redo(void)
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
        _model(model), _editedComponent(editedComponent),
        _newName(newName), _newValue(newValue), _oldOrientation(originalOrientation)
{
    _oldName = editedComponent->getName();
    Resistor* resistor = dynamic_cast<Resistor*>(editedComponent);
    bool isResistor = (nullptr != resistor);
    if (isResistor)
    {
        _oldValue = resistor->getResistanceValue();
    }
    else
    {
        _oldValue = editedComponent->getVoltage();
    }
    _newOrientation = editedComponent->getOrientation();
}

/*!
 * \brief Undo der bearbeiteten Eigenschaften einer Komponente.
 */
void CommandEditComponent::undo(void)
{
    if (_editedComponent != nullptr)
    {
        _editedComponent->setName(_oldName);
        _editedComponent->setOrientation(_oldOrientation);

        Resistor* resistor = dynamic_cast<Resistor*>(_editedComponent);
        bool isAResistor = (nullptr != resistor);
        if (isAResistor)
        {
            resistor->setResistanceValue(_oldValue);
        }
        else
        {
            _editedComponent->setVoltage(_oldValue);
        }
        _model->updateCalc();
    }
}

/*!
 * \brief Redo der bearbeiteten Eigenschaften einer Komponente.
 */
void CommandEditComponent::redo(void)
{
    if (nullptr != _editedComponent)
    {
        _editedComponent->setName(_newName);
        _editedComponent->setOrientation(_newOrientation);

        Resistor* resistor = dynamic_cast<Resistor*>(_editedComponent);
        bool isAResistor = (nullptr != resistor);
        if (isAResistor)
        {
            resistor->setResistanceValue(_newValue);
        }
        else
        {
            _editedComponent->setVoltage(_newValue);
        }
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

/*!
 * \brief Undo des Entfernen einer Komponente.
 */
void CommandDeleteComponent::undo(void)
{
    _model->addComponentWithoutUndo(_deletedComponent);
    for (Connection* connection : _deletedConnections)
    {
        _model->addConnectionWithoutUndo(connection);
    }
    _hasDoneUndo = true;
}

/*!
 * \brief Redo des Entfernen einer Komponente.
 */
void CommandDeleteComponent::redo(void)
{
    _deletedConnections = _model->deleteComponentWithoutUndoAndGetDeletedConnections(_deletedComponent);
    _hasDoneUndo = false;
}

CommandDeleteComponent::~CommandDeleteComponent(void)
{
    if (!_hasDoneUndo)
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

/*!
 * \brief Undo des Entfernen einer Connection.
 */
void CommandDeleteConnection::undo(void)
{
    _hasDoneUndo = true;
    _model->addConnectionWithoutUndo(_deletedConnection);
}

/*!
 * \brief Redo des Entfernen einer Connection.
 */
void CommandDeleteConnection::redo(void)
{
    _hasDoneUndo = false;
    _model->deleteConnectionWithoutUndo(_deletedConnection);
}

CommandDeleteConnection::~CommandDeleteConnection(void)
{
    if (!_hasDoneUndo)
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

CommandDeleteDescription::~CommandDeleteDescription(void)
{
    if (!_hasDoneUndo)
    {
        delete _deletedDescription;
    }
}

/*!
 * \brief Undo des Entfernen eines Textfeldes.
 */
void CommandDeleteDescription::undo(void)
{
    _hasDoneUndo = true;
    _model->addDescriptionFieldWithoutUndo(_deletedDescription);
}

/*!
 * \brief Redo des Entfernen eines Textfeldes.
 */
void CommandDeleteDescription::redo(void)
{
    _hasDoneUndo = false;
    _model->deleteDescriptionWithoutUndo(_deletedDescription);
}

/*
 * _______________________________________________________________________
 * CommandDeleteDescription
 */

CommandDuplicateComponent::CommandDuplicateComponent(NetworkGraphics* model, Component* componentToDuplicate,
                                                     int xPosition, int yPosition) :
        _model(model), _componentToDuplicate(componentToDuplicate), _xPosition(xPosition), _yPosition(yPosition)
{
}

/*!
 * \brief Undo der zuvor kopierten Komponente.
 */
void CommandDuplicateComponent::undo(void)
{
    if (_createdComponent != nullptr)
    {
        _model->deleteComponentWithoutUndoAndGetDeletedConnections(_createdComponent);
    }
}

/*!
 * \brief Redo der kopierten Komente, die enfernt wurde.
 */
void CommandDuplicateComponent::redo(void)
{
    if (_componentToDuplicate != nullptr)
    {
        _createdComponent = _model->duplicateComponentWithoutUndo(_componentToDuplicate, _xPosition, _yPosition);
    }
}

/*
 * _______________________________________________________________________
 * CommandEditDescription
 */

CommandEditDescription::CommandEditDescription(NetworkGraphics* model, DescriptionField* descriptionFieldToEdit,
                                               QString newText) :
        _editDescription(descriptionFieldToEdit), _model(model), _newText(newText),
        _oldText(descriptionFieldToEdit->getText())
{
}

/*!
 * \brief Undo der Bearbeitung eines Textfeldes.
 */
void CommandEditDescription::undo(void)
{
    _editDescription->setText(_oldText);
    _model->update();
}

/*!
 * \brief Redo der Bearbeitung eines Textfeldes.
 */
void CommandEditDescription::redo(void)
{
    _editDescription->setText(_newText);
    _model->update();
}

/*
 * _______________________________________________________________________
 * CommandRotateDescription
 */

CommandRotateComponent::CommandRotateComponent(Component* componentToTurn, NetworkGraphics* model) :
        _componentToTurn(componentToTurn), _model(model)
{
}

/*!
 * \brief Undo des Drehen einer Komponente.
 */
void CommandRotateComponent::undo(void)
{
    //Dreimal rechts = einmal links
    for (int i = 0; i < 3; i++)
    {
        _model->turnComponentRightWithoutUndo(_componentToTurn);
    }
}

/*!
 * \brief Redo des Drehen einer Komponente.
 */
void CommandRotateComponent::redo(void)
{
    _model->turnComponentRightWithoutUndo(_componentToTurn);
}

/*
 * _______________________________________________________________________
 * CommandRotateDescription
 */

CommandCutComponents::CommandCutComponents(NetworkGraphics* model, Component* componentToCut) :
    _model(model), _componentToCut(componentToCut)
{
}

/*!
 * \brief Undo des Ausschneiden einer Komponente.
 */
void CommandCutComponents::undo(void)
{
    _model->addComponentWithoutUndo(_componentToCut);
    _hasDoneUndo = true;
}

/*!
 * \brief Redo des Ausschneiden einer Komponente.
 */
void CommandCutComponents::redo(void)
{
    _model->cutComponentWithoutUndo(_componentToCut);
}

/*
 * _______________________________________________________________________
 * CommandCutDescriptionField
 */

CommandCutDescriptionField::CommandCutDescriptionField(NetworkGraphics *model, DescriptionField *descriptionToCut) :
    _model(model), _descriptionFieldToCut(descriptionToCut)
{
}

/*!
 * \brief Undo des Ausschneiden eines Textfeldes.
 */
void CommandCutDescriptionField::undo(void)
{
    _model->addDescriptionFieldWithoutUndo(_descriptionFieldToCut);
    _hasDoneUndo = true;
}

/*!
 * \brief Redo des Ausschneiden eines Textfeldes.
 */
void CommandCutDescriptionField::redo(void)
{
    _model->cutDescriptionWithoutUndo(_descriptionFieldToCut);
}

/*
 * _______________________________________________________________________
 * CommandDuplicateDescription
 */

CommandDuplicateDescription::CommandDuplicateDescription(NetworkGraphics *model, DescriptionField *descriptionToDuplicate, int xPosition, int yPosition) :
    _model(model), _descriptionToDuplicate(descriptionToDuplicate), _xPosition(xPosition), _yPosition(yPosition)
{
}

/*!
 * \brief Undo des Duplizieren eines Textfeldes.
 */
void CommandDuplicateDescription::undo(void)
{
    _model->deleteDescriptionWithoutUndo(_createdDescription);
}

/*!
 * \brief Redo des Duplizieren eines Textfeldes.
 */
void CommandDuplicateDescription::redo(void)
{

   _createdDescription = _model->addDescriptionFieldWithoutUndo(QPointF(_xPosition,_yPosition), false, _descriptionToDuplicate->getText());
}
