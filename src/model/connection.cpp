#include "connection.h"
#include <model/component.h>

//TODO: isVertical only for testing true
Connection::Connection(int xStart, int yStart, int xEnd, int yEnd) :
    Component(xStart, yStart, true, 3, 2)
{
    _xStart = xStart;
    _yStart = yStart;
    _xEnd   = xEnd;
    _yEnd   = yEnd;
}

int Connection::getXStartPosition() {
    return 0;
}

int Connection::getYStartPosition() {
    return 0;
}
