#include "resistor.h"


Resistor::Resistor(QString name, int value, int x, int y, bool isVertical)
    : Component(x, y, isVertical, 1, 2),
    _name(name), _value(value)
{
    //Properties of Resistor
    //_countPorts 2
    //_componentType 1
}

Resistor::~Resistor()
{
    _resistorCount--;
}

void Resistor::show(QGraphicsScene *scene)
{
    if(_isVertical) {
        //Resistor has length of 120 and width of 60
        scene->addRect(_xPosition, _yPosition + 20, 40, 60);
        scene->addLine(_xPosition + 20, _yPosition + 0, _xPosition + 20, _yPosition + 20);
        scene->addLine(_xPosition + 20, _yPosition + 80, _xPosition + 20, _yPosition + 100);
    } else {
        scene->addRect(_xPosition - 30, _yPosition - 20, 60, 40);
        scene->addLine(_xPosition -30 , _yPosition + 0, _xPosition - 50, _yPosition + 0);
        scene->addLine(_xPosition + 30, _yPosition + 0, _xPosition + 60, _yPosition + 0);
    }
}

void Resistor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

int Resistor::getXStartPosition()
{
    return int(this->getXPosition() - (this->getWidth() / 2));
}

int Resistor::getYStartPosition()
{
    return int(this->getYPosition() - (this->getHeight() / 2));
}
