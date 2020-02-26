//
// Created by erics on 026, 26, Februar.
//

#include "commands.h"
#include <model/networkgraphics.h>

CommandAddComponent::CommandAddComponent(NetworkGraphics* model, QPointF gridPosition, Component::ComponentType componentType,
                                         bool componentIsVertical) :
        _model(model), _gridPosition(gridPosition), _componentType(componentType),
        _componentIsVertical(componentIsVertical)
{

}

void CommandAddComponent::undo()
{
    _model->deleteComponent(_createdComponent);
}

void CommandAddComponent::redo()
{
    Component* createdComponent = nullptr;

    if (_model->isThereAComponentOrADescription(_gridPosition))
    {
        return;
    }

    if (Component::ComponentType::Resistor == _componentType)
    {

        createdComponent = _model->addResistor("", 100, _gridPosition.x(), _gridPosition.y(), _componentIsVertical);
    }
    else if (Component::ComponentType::PowerSupply == _componentType)
    {
        createdComponent = _model->addPowerSupply("", _gridPosition.x(), _gridPosition.y(), _componentIsVertical);
    }

    if (!_model->isLoading())
    {
        _model->updateCalc();
    }
    _createdComponent = createdComponent;
}


