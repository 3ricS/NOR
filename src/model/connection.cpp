#include "connection.h"
#include <model/component.h>

Connection::Connection(int xStart, int yStart, int xEnd, int yEnd) : Component(xStart, yStart, 10, 5, 0, 2)
{

}

int Connection::getXStartPosition()
{
    return 0;
}

int Connection::getYStartPosition()
{
    return 0;
}
