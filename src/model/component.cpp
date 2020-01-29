#include "component.h"

Component::Component(int x, int y, bool isVertical, ComponentType componentTyp, int countPorts)
    : QGraphicsItem(nullptr),
      _id(++_count), _xPosition(x), _yPosition(y), _isVertical(isVertical), _componentType(componentTyp), _countPorts(countPorts)
{

}

QRectF Component::boundingRect() const
{
    return QRectF(_xPosition - 50, _yPosition - 50, 100, 100);
}

