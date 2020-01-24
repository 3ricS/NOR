#ifndef COMPONENT_H
#define COMPONENT_H

#include <QObject>

class Component : public QObject
{
    Q_OBJECT
public:
    Component(int x, int y, int hight, int width,  int componentTyp, int countPorts);

    //getter
    int getComponentType() {return _componentTyp;}
    int getXPosition() {return _xPosition;}
    int getYPosition() {return _yPosition;}

    virtual int getXStartPosition() = 0;
    virtual int getYStartPosition() = 0;

    int getWidth() const;
    int getHight() const;

signals:
    void remove(void);

private:
    static int _count;
    const int _id;
    int _xPosition;
    int _yPosition;
    int _hight;
    int _width;

    //_componentTyp gets the type of the object
    //0: Line, 1: Resistor, 2: Power Supply
    const int _componentTyp;
    const int _countPorts;
};

#endif // COMPONENT_H
