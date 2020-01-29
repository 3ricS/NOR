#include "resistor.h"


Resistor::Resistor(QString name, int value, int x, int y, bool isVertical)
    : Component(x, y, isVertical, Component::ComponentType::Resistor, 2),
    _name(name), _value(value)
{
    //Properties of Resistor
    //_countPorts 2
    //_componentType 1

    _resistorCount++;
}

Resistor::~Resistor()
{
    _resistorCount--;
}

void Resistor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawText(_xPosition - 40, _yPosition - 40, _name);
    if(_isVertical)
    {
        //Resistor has length of 120 and width of 60
        painter->drawRect(_xPosition - 20, _yPosition - 30, 40, 60);
        painter->drawLine(_xPosition, _yPosition - 30, _xPosition, _yPosition - 50);
        painter->drawLine(_xPosition, _yPosition + 30, _xPosition, _yPosition + 50);
    } else {
        painter->drawRect(_xPosition - 30, _yPosition - 20, 60, 40);
        painter->drawLine(_xPosition -30 , _yPosition + 0, _xPosition - 50, _yPosition + 0);
        painter->drawLine(_xPosition + 30, _yPosition + 0, _xPosition + 60, _yPosition + 0);
    }
}

int Resistor::getXStartPosition()
{
    return int(this->getXPosition() - 50);
}

int Resistor::getYStartPosition()
{
    return int(this->getYPosition() - 50);
}
