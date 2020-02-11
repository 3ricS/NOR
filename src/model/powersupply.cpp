#include "powersupply.h"

PowerSupply::PowerSupply(QString name, int x, int y, bool isVertical) :
        Component(x, y, isVertical, name, 0, Component::ComponentType::PowerSupply, 2)
{

}

void PowerSupply::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    paintInformations(painter);

    if (_isVertical)
    {
        painter->drawEllipse(_xPosition - 30, _yPosition - 30, 60, 60);
        painter->drawLine(_xPosition, _yPosition + 50, _xPosition, _yPosition - 50);
    }
    else
    {
        painter->drawEllipse(_xPosition - 30, _yPosition - 30, 60, 60);
        painter->drawLine(_xPosition - 50, _yPosition, _xPosition + 50, _yPosition);
    }
}

void PowerSupply::paintInformations(QPainter* painter)
{
    painter->drawText(_xPosition - 40, _yPosition - 40, _name);
}

