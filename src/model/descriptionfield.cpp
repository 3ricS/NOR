#include "descriptionfield.h"

DescriptionField::DescriptionField(int x, int y, int id, QString text)
    : QGraphicsItem(nullptr),
    _xPosition(x), _yPosition(y), _id(id)
{
    if(text != "")
    {
        _text = text;
    }
}

void DescriptionField::setPosition(QPointF gridPosition)
{
    _xPosition = gridPosition.x();
    _yPosition = gridPosition.y();
}

QRectF DescriptionField::boundingRect() const
{
    return QRectF (_xPosition - 100, _yPosition - 50, 150, 100);
}

void DescriptionField::paint(QPainter *painter,[[maybe_unused]] const QStyleOptionGraphicsItem *option, [[maybe_unused]] QWidget *widget)
{
    painter->drawText(QRectF (_xPosition - 40, _yPosition - 40, 80, 80), _text);
}
