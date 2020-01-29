#include "component.h"

Component::Component(int x, int y, bool isVertical, ComponentType componentTyp, int countPorts)
    : QGraphicsItem(nullptr),
      _id(++_count), _xPosition(x), _yPosition(y), _isVertical(isVertical), _componentType(componentTyp), _countPorts(countPorts)
{

}

QRectF Component::boundingRect() const
{
}

