#include "component.h"

Component::Component(int x, int y)
{
    _id = ++_count;
    _xPosition = x;
    _yPosition = y;
}
