#include "connection.h"

Connection::Connection(int xStart, int yStart, int xEnd, int yEnd)
{
    _xStart = xStart;
    _yStart = yStart;
    _xEnd   = xEnd;
    _yEnd   = yEnd;
}

void Connection::show(QGraphicsScene* scene)
{
    //first calculate the coordinates of the middle
    int xMiddle = _xStart + 0.5 * (_xEnd - _xStart);
    int yMiddle = _yStart + 0.5 * (_yEnd - _yStart);

    //draw the four lines to make the connection square
    scene->addLine(_xStart, _yStart, _xStart, yMiddle);
    scene->addLine(_xStart, yMiddle, xMiddle, yMiddle);
    scene->addLine(xMiddle, yMiddle, _xEnd, yMiddle);
    scene->addLine(_xEnd, yMiddle, _xEnd, _yEnd);
}

