#include "resistor.h"


Resistor::Resistor(QString name, int value, int x, int y)
    : Component(x, y, 1, 2)
{
    //Properties of Resistor
    //_countPorts 2
    //_componentType 1

    _name = name;
    _value = value;
}
