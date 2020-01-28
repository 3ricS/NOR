#include "resistor.h"


Resistor::Resistor(QString name, int value, int x, int y, bool isVertical)
    : Component(x, y, isVertical, 1, 2),
    _name(name), _value(value)
{
    //Properties of Resistor
    //_countPorts 2
    //_componentType 1
}

Resistor::~Resistor()
{
    _resistorCount--;
}

int Resistor::getXStartPosition()
{
    return int(this->getXPosition() - (this->getWidth() / 2));
}

int Resistor::getYStartPosition()
{
    return int(this->getYPosition() - (this->getHeight() / 2));
}
