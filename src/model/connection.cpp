#include "connection.h"

Connection::Connection(Component* componentA, Component::Port componentAPort, Component* componentB,
                       Component::Port componentBPort) :
        QGraphicsItem(nullptr),
        _componentA(componentA), _portA(componentAPort),
        _componentB(componentB), _portB(componentBPort)
{
}

void Connection::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    //TODO: Funktion mit mehreren Rückgabewerten für xStart, xEnd, ...
    QPointF start = _componentA->getPortPosition(_portA);
    QPointF end = _componentB->getPortPosition(_portB);
    int xStart = start.x();
    int yStart = start.y();
    int xEnd = end.x();
    int yEnd = end.y();

    //first calculate the coordinates of the middle
    int xMiddle = xStart + 0.5 * (xEnd - xStart);
    int yMiddle = yStart + 0.5 * (yEnd - yStart);

    //draw the four lines to make the connection square
    painter->drawLine(xStart, yStart, xStart, yMiddle);
    painter->drawLine(xStart, yMiddle, xMiddle, yMiddle);
    painter->drawLine(xMiddle, yMiddle, xEnd, yMiddle);
    painter->drawLine(xEnd, yMiddle, xEnd, yEnd);
}

QRectF Connection::boundingRect(void) const
{
    QPointF start = _componentA->getPortPosition(_portA);
    QPointF end = _componentB->getPortPosition(_portB);
    int xStart = start.x();
    int yStart = start.x();
    int xEnd = end.x();
    int yEnd = end.y();

    return QRectF(std::min(xStart, xEnd), std::min(yStart, yEnd), abs(xEnd - xStart), abs(yEnd - yStart));
}
