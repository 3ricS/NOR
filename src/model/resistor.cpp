#include "resistor.h"


Resistor::Resistor(QString name, int value, int x, int y)
    : Component(x, y)
{
    _name = name;
    _value = value;

    //Properties of Resistor
    _countPorts = 2;
    _componentType = 1;
}
