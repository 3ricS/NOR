#include "component.h"

Component::Component(int x, int y, int hight, int width,  int componentTyp, int countPorts)
    : _id(++_count), _xPosition(x), _yPosition(y), _hight(hight), _width(width), _componentTyp(componentTyp), _countPorts(countPorts)
{

}

int Component::getWidth() const
{
    return _width;
}

int Component::getHight() const
{
    return _hight;
}
