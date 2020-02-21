#include "descriptionfield.h"

DescriptionField::DescriptionField(int x, int y, int id)
    : QGraphicsItem(nullptr),
    _xPosition(x), _yPosition(y), _id(id)
{
}

QRectF DescriptionField::boundingRect() const
{
    return QRectF (_xPosition - 100, _yPosition - 50, 150, 100);
}

void DescriptionField::paint(QPainter *painter,[[maybe_unused]] const QStyleOptionGraphicsItem *option, [[maybe_unused]] QWidget *widget)
{
    painter->drawText(QRectF (_xPosition - 50, _yPosition - 50, 100, 100), _text);
}
