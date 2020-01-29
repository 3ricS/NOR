#ifndef COMPONENT_H
#define COMPONENT_H

#include <QGraphicsItem>
#include <QGraphicsScene>

class Component : public QGraphicsItem
{
public:
    Component(int _xPosition, int _yPosition, bool isVertical, int componentTyp, int countPorts);

    QRectF boundingRect() const;

    //getter
    int getComponentType() {return _componentType;}
    int getXPosition() {return _xPosition;}
    int getYPosition() {return _yPosition;}
    bool isVertical() {return _isVertical;}

    //setter
    void setVertical(bool orientation){_isVertical = orientation;}

    //virtual Methoden
    virtual int getXStartPosition() = 0;    //TODO: Wenn Gitter dann, nicht mehr virtual direkt in Compononet.cpp implementiert
    virtual int getYStartPosition() = 0;    //TODO: Wenn Gitter dann, nicht mehr virtual direkt in Compononet.cpp implementiert
    virtual void show(QGraphicsScene* scene) = 0;
    //virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    int _xPosition;
    int _yPosition;
    bool _isVertical;

private:
    static int _count;
    const int _id;

    //_componentTyp gets the type of the object
    //0: Line, 1: Resistor, 2: Power Supply
    const int _componentType;
    const int _countPorts;
};

#endif // COMPONENT_H
