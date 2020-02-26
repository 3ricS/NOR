//
// Created by erics on 026, 26, Februar.
//

#ifndef NOR_COMMANDS_H
#define NOR_COMMANDS_H

#include <QUndoCommand>
#include <QPointF>

#include <model/component.h>

class NetworkGraphics;

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


#endif //NOR_COMMANDS_H
