#ifndef RESISTOR_H
#define RESISTOR_H

#include "component.h"
#include <QString>

class Resistor : public Component
{
public:
    Resistor();

private:
    QString _name;
    int _value;


    // Component interface
signals:
    void draw();
    void remove();
};

#endif // RESISTOR_H
