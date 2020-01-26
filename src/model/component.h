#ifndef COMPONENT_H
#define COMPONENT_H

#include <QObject>

class Component : public QObject
{
    Q_OBJECT
public:
    Component(int x, int y, int componentTyp, int countPorts);

    //getter
    int getComponentType() {return _componentType;}
    int getXPosition() {return _xPosition;}
    int getYPosition() {return _yPosition;}

    virtual int getXStartPosition() = 0;
    virtual int getYStartPosition() = 0;

signals:
    void remove(void);

private:
    static int _count;
    const int _id;
    int _xPosition;
    int _yPosition;

    //_componentTyp gets the type of the object
    //0: Line, 1: Resistor, 2: Power Supply
    const int _componentType;
    const int _countPorts;
};

#endif // COMPONENT_H
