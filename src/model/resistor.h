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
    int getWidth() {return _width;}
    int getHeight() {return _height;}

    void setResistance(int resistance) {_value = resistance;}


// Component interface
signals:
    void draw();
    void remove();

private:
    static int _resistorCount;
    static constexpr int _height = 100;
    static constexpr int _width = 40;

    QString _name;
    int _value;
};

#endif // RESISTOR_H
