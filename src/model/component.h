#ifndef COMPONENT_H
#define COMPONENT_H

#include <QString>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>

class Component : public QGraphicsItem
{
public:
    //TODO: darf amn das groß schreiben
    enum ComponentType{Resistor, PowerSupply};
    Component(int _xPosition, int _yPosition, bool isVertical, ComponentType componentTyp, int countPorts);

    //TODO: boundingRect muss noch beschrieben werden
    QRectF boundingRect() const;

    //getter
    int getComponentType(void) const {return _componentType;}
    int getXPosition(void) const {return _xPosition;}
    int getYPosition(void) const {return _yPosition;}
    bool isVertical(void) const {return _isVertical;}

    //setter
    void setVertical(bool orientation) {_isVertical = orientation;}

    //virtual Methoden
    virtual int getXStartPosition() = 0;    //TODO: Wenn Gitter dann, nicht mehr virtual direkt in Compononet.cpp implementiert
    virtual int getYStartPosition() = 0;    //TODO: Wenn Gitter dann, nicht mehr virtual direkt in Compononet.cpp implementiert

protected:
    //Variables
    int _xPosition;
    int _yPosition;
    bool _isVertical;

    //virtuelle Methoden
    virtual void paintInformations(QPainter* painter) = 0;

private:
    static int _count;
    const int _id;

    //_componentTyp gets the type of the object
    const ComponentType _componentType;
    const int _countPorts;
};

#endif // COMPONENT_H
