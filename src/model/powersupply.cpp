#include "powersupply.h"

PowerSupply::PowerSupply(QString name, int x, int y) :
    Component(x, y, 0, 0, 2, 2)
{
    _name = name;
}

int PowerSupply::getXStartPosition()
{
    return this->getXPosition() - (this->getWidth() / 2);
}

int PowerSupply::getYStartPosition()
{
    return this->getYPosition() - (this->getHeight() / 2);
}
