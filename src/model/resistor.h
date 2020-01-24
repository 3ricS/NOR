#ifndef RESISTOR_H
#define RESISTOR_H

#include <QString>

#include <model/component.h>


class Resistor : public Component
{
public:
    Resistor(QString _name, int value, int x, int y);
    ~Resistor();

    //getter
    virtual int getXStartPosition() override;
    virtual int getYStartPosition() override;

    static int getResistorCount() {return _resistorCount;}

// Component interface
signals:
    void draw();
    void remove();

private:
    static int _resistorCount;

    QString _name;
    int _value;
};

#endif // RESISTOR_H
