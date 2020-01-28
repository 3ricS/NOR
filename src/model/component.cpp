#include "component.h"

Component::Component(int x, int y, bool isVertical, int componentTyp, int countPorts)
    : _id(++_count), _xPosition(x), _yPosition(y), _isVertical(isVertical), _componentType(componentTyp), _countPorts(countPorts)
{

}
