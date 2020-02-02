#include "component.h"

Component::Component(int x, int y, bool isVertical, QString name, int value, ComponentType componentTyp, int countPorts)
        : QGraphicsItem(nullptr),
          _id(++_count), _xPosition(x), _yPosition(y), _isVertical(isVertical),
          _name(name), _value(value),
          _componentType(componentTyp), _countPorts(countPorts)
{

}

QRectF Component::boundingRect(void) const
{
    //TODO: Zoomfaktor hier einfügen
    return QRectF(_xPosition - 50, _yPosition - 50, 100, 100);
}

int Component::getPortPositionXOrY(int positionValue, Port port, bool isX) const
{
    //Portangabe in Faktor umrechnen
    int factor = 0;
    switch (port)
    {
        case A:
            factor = -1;
            break;
        case B:
            factor = 1;
            break;
    }

    if((_isVertical && !isX) || (!_isVertical && isX))
    {
        //TODO: Zoomfaktor einfügen
        return positionValue + factor * 50;
    } else
    {
        return positionValue;
    }
}

Component::Port Component::getPort(QPointF position) const
{
    bool xEqualPortA = (position.x() > getPortPositionXOrY(_xPosition, Port::A, true) - _hitBoxSize &&
                        position.x() < getPortPositionXOrY(_xPosition, Port::A, true) + _hitBoxSize);
    bool yEqualPortA = (position.y() > getPortPositionXOrY(_yPosition, Port::A, false) - _hitBoxSize &&
                        position.y() < getPortPositionXOrY(_yPosition, Port::A, false) + _hitBoxSize);
    bool xEqualPortB = (position.x() > getPortPositionXOrY(_xPosition, Port::B, true) - _hitBoxSize &&
                        position.x() < getPortPositionXOrY(_xPosition, Port::B, true) + _hitBoxSize);
    bool yEqualPortB = (position.y() > getPortPositionXOrY(_yPosition, Port::B, false) - _hitBoxSize &&
                        position.y() < getPortPositionXOrY(_yPosition, Port::B, false) + _hitBoxSize);

    if(xEqualPortA && yEqualPortA)
    {
        return Port::A;
    } else if(xEqualPortB && yEqualPortB)
    {
        return Port::B;
    } else
    {
        return Port::null;
    }
}

bool Component::hasPortAtPosition(QPointF position) const
{
    Port foundPort = getPort(position);
    return Port::null != foundPort;
}

QPointF Component::getPortPosition(Component::Port port) const
{
    //TODO: Zoomfaktor einfügen
    return QPointF(getPortPositionXOrY(_xPosition, port, true), getPortPositionXOrY(_yPosition, port, false));
}

void Component::setPosition(QPointF gridPosition)
{
    _xPosition = gridPosition.toPoint().x();
    _yPosition = gridPosition.toPoint().y();
}
