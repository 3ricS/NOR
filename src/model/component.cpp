#include <view/editview.h>
#include "component.h"

Component::Component(int x, int y, bool isVertical, QString name, int value, ComponentType componentTyp, int id)
        : QGraphicsItem(nullptr),
          _id(id), _xPosition(x), _yPosition(y), _isVertical(isVertical),
          _name(name), _value(value),
          _componentType(componentTyp)
{
    if (isVertical)
    {
        _orientation = Orientation::top;
    }
    else
    {
        _orientation = Orientation::left;
    }
}

QRectF Component::boundingRect(void) const
{
    //TODO: Zoomfaktor hier einfügen
    return QRectF(_xPosition - 100, _yPosition - 50, 150, 100);
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

    if ((_isVertical && !isX) || (!_isVertical && isX))
    {
        //TODO: Zoomfaktor einfügen
        return positionValue + factor * 50;
    }
    else
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

    if (xEqualPortA && yEqualPortA)
    {
        return Port::A;
    }
    else if (xEqualPortB && yEqualPortB)
    {
        return Port::B;
    }
    else
    {
        return Port::null;
    }
}

/*!
* \brief Prüft ob an der ausgewählten Position ein Port befindet
*
* Die Methode kriegt die zu prüfende Position übergeben.
*
*
*/
bool Component::hasPortAtPosition(QPointF position) const
{
    Port foundPort = getPort(position);
    return Port::null != foundPort;
}

/*!
* \brief Wandelt den Integer in einen Komponententyp
*
* Die Methode bekommt einen Integer componentType übergeben.
* Es wird geprüft ob es sich bei dem componentType um einen Widerstand oder eine Spannungsquelle handelt.
* Die Methode gibt den weweiligen ComponentType des zu prüfenden Integer zurück.
*/
Component::ComponentType Component::integerToComponentType(int componentType)
{
    if (0 == componentType)
    {
        return ComponentType::Resistor;
    }
    else if (1 == componentType)
    {
        return ComponentType::PowerSupply;
    }
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

void Component::setOrientation(Component::Orientation newOrientation)
{
    _orientation = newOrientation;
    if (Orientation::left == newOrientation || Orientation::right == newOrientation)
    {
        _isVertical = false;
    } else {
        _isVertical = true;
    }
}
