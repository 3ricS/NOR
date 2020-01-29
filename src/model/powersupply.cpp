#include "powersupply.h"

PowerSupply::PowerSupply(QString name, int x, int y, bool isVertical) :
    Component(x, y, isVertical, Component::ComponentType::PowerSupply, 2),
    _name(name)
{
    _powerSupplyCount++;
}

void PowerSupply::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawText(_xPosition - 40, _yPosition - 40, _name);
    if(_isVertical)
    {
        painter->drawEllipse(_xPosition - 30, _yPosition - 30, 60, 60);
        painter->drawLine(_xPosition, _yPosition + 60, _xPosition, _yPosition - 60);
    }
    else
    {
        painter->drawEllipse(_xPosition - 30, _yPosition - 30, 60, 60);
        painter->drawLine(_xPosition - 60, _yPosition, _xPosition + 60, _yPosition);
    }
}

int PowerSupply::getXStartPosition(void)
{
    return this->getXPosition() - 50;
}

int PowerSupply::getYStartPosition(void)
{
    return this->getYPosition() - 50;
}

