//
// Created by erics on 026, 26, Februar.
//

#ifndef NOR_COMMANDS_H
#define NOR_COMMANDS_H

#include <QUndoCommand>
#include <QPointF>

#include <model/component.h>
#include <model/componentport.h>

class NetworkGraphics;
class Connection;
class DescriptionField;

class CommandAddComponent : public QUndoCommand
{
public:
    CommandAddComponent(NetworkGraphics* model, QPointF gridPosition, Component::ComponentType componentType, bool componentIsVertical);

    void undo() override;
    void redo() override;

    Component* getCreatedComponent() {return _createdComponent;}

private:
    Component*                  _createdComponent = nullptr;
    NetworkGraphics*            _model = nullptr;
    QPointF                     _gridPosition;
    Component::ComponentType    _componentType;
    bool                        _componentIsVertical;
    QList<Connection*>          _deletedConnections;
};



class CommandAddConnection : public QUndoCommand
{
public:
    CommandAddConnection(NetworkGraphics* model, ComponentPort componentPortA, ComponentPort componentPortB);

    void undo() override;
    void redo() override;

private:
    NetworkGraphics*    _model = nullptr;
    Connection*         _createdConnection = nullptr;
    ComponentPort       _componentPortA;
    ComponentPort       _componentPortB;
};


class CommandMoveComponent : public QUndoCommand
{
public:
    CommandMoveComponent(NetworkGraphics* model, Component* componentToMove,
                         DescriptionField* descriptionToMove, QPointF gridPosition);

    void undo() override;
    void redo() override;

private:
    NetworkGraphics*    _model = nullptr;
    Component*          _componentToMove = nullptr;
    DescriptionField*   _descriptionToMove = nullptr;
    QPointF             _gridEndPosition;
    QPointF             _gridStartPosition;
};



class CommandEditComponent : public QUndoCommand {
public:
    CommandEditComponent(NetworkGraphics* model, Component* editedComponent,
                         Component::Orientation originalOrientation, QString newName,
                         double newValue);

    void undo() override;
    void redo() override;

private:
    NetworkGraphics*    _model = nullptr;
    Component*          _editedComponent = nullptr;
    QString             _newName;
    QString             _oldName;
    double              _newValue;
    double              _oldValue;
    Component::Orientation  _oldOrientation;
    Component::Orientation  _newOrientation;
};

class CommandDeleteComponent : public QUndoCommand
{
public:
    CommandDeleteComponent(NetworkGraphics* model, Component* componentToDelete);
    ~CommandDeleteComponent();

    void undo() override;
    void redo() override;

private:
    NetworkGraphics*    _model = nullptr;
    Component*          _deletedComponent = nullptr;
    QList<Connection*>  _deletedConnections;
    bool                _hasDoneUndo = false;
};


#endif //NOR_COMMANDS_H
