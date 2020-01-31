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
    //TODO: darf amn das groß schreiben?
    enum ComponentType{Resistor, PowerSupply};
    Component(int x, int y, bool isVertical, int value, ComponentType componentTyp, int countPorts);

    QRectF boundingRect() const;

    //getter
    int getComponentType(void) const {return _componentType;}
    int getXPosition(void) const {return _xPosition;}
    int getYPosition(void) const {return _yPosition;}
    bool isVertical(void) const {return _isVertical;}

    QString getName(void) const {return _name;}
    int getValue(void) const {return _value;}

    //setter
    void setVertical(bool orientation) {_isVertical = orientation;}
    void setName(QString name) {_name = name;}
    void setValue(int newValue) {_value = newValue;}

    //virtual Methoden
    //virtual int getXStartPosition(void) = 0;    //TODO: Wenn Gitter dann, nicht mehr virtual direkt in Compononet.cpp implementiert
    //virtual int getYStartPosition(void) = 0;    //TODO: Wenn Gitter dann, nicht mehr virtual direkt in Compononet.cpp implementiert

protected:
    //Variables
    int _xPosition;
    int _yPosition;
    bool _isVertical;

    QString _name;
    int _value;

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
