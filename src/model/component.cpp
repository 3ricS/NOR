#include "component.h"

Component::Component(int x, int y, int xEnd, int yEnd, int componentTyp, int countPorts)
    : _id(++_count), _xPosition(x), _yPosition(y), _xEndPosition(xEnd), _yEndPosition(yEnd), _componentType(componentTyp), _countPorts(countPorts)
{

}
