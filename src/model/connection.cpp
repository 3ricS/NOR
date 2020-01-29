#include "connection.h"

Connection::Connection(int xStart, int yStart, int xEnd, int yEnd) :
    QGraphicsItem(nullptr)
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

void Connection::paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //first calculate the coordinates of the middle
    int xMiddle = _xStart + 0.5 * (_xEnd - _xStart);
    int yMiddle = _yStart + 0.5 * (_yEnd - _yStart);

    //draw the four lines to make the connection square
    painter->drawLine(_xStart, _yStart, _xStart, yMiddle);
    painter->drawLine(_xStart, yMiddle, xMiddle, yMiddle);
    painter->drawLine(xMiddle, yMiddle, _xEnd, yMiddle);
    painter->drawLine(_xEnd, yMiddle, _xEnd, _yEnd);
}

QRectF Connection::boundingRect() const
{
    return QRectF(std::min(_xStart, _xEnd), std::min(_yStart, _yEnd), abs(_xEnd - _xStart), abs(_yEnd - _yStart));
}

