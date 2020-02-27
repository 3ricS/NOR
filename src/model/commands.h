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
    NetworkGraphics*    _model;
    Component*          _componentToMove;
    DescriptionField*   _descriptionToMove;
    QPointF             _gridEndPosition;
    QPointF             _gridStartPosition;
};



#endif //NOR_COMMANDS_H
