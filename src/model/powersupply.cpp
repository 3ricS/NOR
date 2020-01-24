#include "powersupply.h"

PowerSupply::PowerSupply(QString name, int x, int y) :
    Component(x, y, 100, 100,  2, 2) //wrong values, only for testing!
{

}

int PowerSupply::getXStartPosition()
{
    return this->getXPosition() - (this->getWidth() / 2);
}

int PowerSupply::getYStartPosition()
{
    return this->getYPosition() - (this->getHight() / 2);
}
