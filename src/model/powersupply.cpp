#include "powersupply.h"

PowerSupply::PowerSupply(QString name, int x, int y, bool isVertical) :
    Component(x, y, isVertical, 2, 2),
    _name(name)
{

}

void PowerSupply::show(QGraphicsScene* scene)
{
    //Power Supply has length of 120 and width of 60
    if(_isVertical) {
        scene->addEllipse(_xPosition - 30, _yPosition - 30, 60, 60);
        scene->addLine(_xPosition, _yPosition + 60, _xPosition, _yPosition - 60);
    } else {
        scene->addEllipse(_xPosition - 30, _yPosition - 30, 60, 60);
        scene->addLine(_xPosition - 60, _yPosition, _xPosition + 60, _yPosition);
    }
}

void PowerSupply::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
   // painter->setBrush(Qt::black);
    painter->drawEllipse(_xPosition - 30, _yPosition - 30, 60, 60);
    painter->drawLine(_xPosition, _yPosition + 60, _xPosition, _yPosition - 60);

}

int PowerSupply::getXStartPosition()
{
    return this->getXPosition() - (this->getWidth() / 2);
}

int PowerSupply::getYStartPosition()
{
    return this->getYPosition() - (this->getHeight() / 2);
}

