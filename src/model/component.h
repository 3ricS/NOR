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
    enum Port{A, B, null};
    Component(int x, int y, bool isVertical, QString name, int value, ComponentType componentTyp, int countPorts);

    QRectF boundingRect(void) const;
    bool hasPortAtPosition(QPointF position) const;

    //getter
    int getComponentType(void) const {return _componentType;}
    int getXPosition(void) const {return _xPosition;}
    int getYPosition(void) const {return _yPosition;}
    QString getName(void) const {return _name;}
    int getValue(void) const {return _value;}
    bool isVertical(void) const {return _isVertical;}

    Component::Port getPort(QPointF position) const;
    QPointF getPortPosition(Port port) const;

    //setter
    void setVertical(bool orientation) {_isVertical = orientation;}
    void setName(QString name) {_name = name;}
    void setValue(int newValue) {_value = newValue;}
    void setPosition(QPointF gridPosition);

    static constexpr int _hitBoxSize = 20;

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
    int getPortPositionXOrY(int positionValue, Port port, bool isX) const;

    int _count = 0;
    const int _id;

    //_componentTyp gets the type of the object
    const ComponentType _componentType;
    const int _countPorts;
};

#endif // COMPONENT_H
