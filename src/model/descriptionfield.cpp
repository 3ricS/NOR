#include "descriptionfield.h"

DescriptionField::DescriptionField(int x, int y, int id)
    : QGraphicsItem(nullptr),
    _xPosition(x), _yPosition(y), _id(id)
{
}


void DescriptionField::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawText(_xPosition, _yPosition, _text);
}
