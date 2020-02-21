#include "descriptionfield.h"

DescriptionField::DescriptionField(int x, int y, int id)
    : QGraphicsItem(nullptr),
    _xPosition(x), _yPosition(y), _id(id)
{
}

QRectF DescriptionField::boundingRect() const
{
    return QRectF();
}

void DescriptionField::paint(QPainter *painter,[[maybe_unused]] const QStyleOptionGraphicsItem *option, [[maybe_unused]] QWidget *widget)
{
    painter->drawText(_xPosition, _yPosition, _text);
}
