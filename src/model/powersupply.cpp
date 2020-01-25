#include "powersupply.h"

PowerSupply::PowerSupply(QString name, int x, int y) :
    Component(x, y, 120, 60,  2, 2)
{
    _name = name;
}

int PowerSupply::getXStartPosition()
{
    return this->getXPosition() - (this->getWidth() / 2);
}

int PowerSupply::getYStartPosition()
{
    return this->getYPosition() - (this->getHight() / 2);
}
