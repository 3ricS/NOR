#include "component.h"

Component::Component(int x, int y, bool isVertical, int value, ComponentType componentTyp, int countPorts)
    : QGraphicsItem(nullptr),
      _id(++_count), _xPosition(x), _yPosition(y), _isVertical(isVertical),
      _componentType(componentTyp), _countPorts(countPorts), _value(value)
{

}

QRectF Component::boundingRect() const
{
    //TODO: Zoomfaktor hier einfügen
    return QRectF(_xPosition - 50, _yPosition - 50, 100, 100);
}

