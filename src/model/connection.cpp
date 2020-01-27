#include "connection.h"
#include <model/component.h>

Connection::Connection(int xStart, int yStart, int xEnd, int yEnd) :
    Component(xStart, yStart, xEnd, yEnd, 3, 2)
{
    _xStart = xStart;
    _yStart = yStart;
    _xEnd   = xEnd;
    _yEnd   = yEnd;
}
