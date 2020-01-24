#ifndef COMPONENT_H
#define COMPONENT_H

#include <QObject>

class Component : public QObject
{
    Q_OBJECT
public:
    Component(int x, int y, int componentTyp, int countPorts);

    //getter
    int getComponentType() {return _componentTyp;}
    int getXPosition() {return _xPosition;}
    int getYPosition() {return _yPosition;}

signals:
    void remove(void);    

private:
    static int _count;
    const int _id;
    int _xPosition;
    int _yPosition;

    //_componentTyp gets the type of the object
    //0: Line, 1: Resistor, 2: Power Supply
    const int _componentTyp;
    const int _countPorts;
};

#endif // COMPONENT_H
