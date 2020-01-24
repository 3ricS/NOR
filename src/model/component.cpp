#include "component.h"

Component::Component(int x, int y, int componentTyp, int countPorts)
    : _id(++_count), _xPosition(x), _yPosition(y), _componentTyp(componentTyp), _countPorts(countPorts)
{

}
