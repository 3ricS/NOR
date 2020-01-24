#ifndef COMPONENT_H
#define COMPONENT_H

#include <QObject>

class Component : public QObject
{
    Q_OBJECT
public:
    Component(int x, int y);

    //getter
    int getComponentType() {return _componentType;}
    int getXPosition() {return _xPosition;}
    int getYPosition() {return _yPosition;}

signals:
    void remove(void);

protected:
    int _countPorts;

    //_componentTyp gets the type of the object
    //0: Line, 1: Resistor, 2: Power Supply
    int _componentType;

private:
    static int _count;
    int _id;
    int _xPosition;
    int _yPosition;
};

#endif // COMPONENT_H
