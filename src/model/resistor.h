#ifndef RESISTOR_H
#define RESISTOR_H

#include <model/component.h>

class Resistor : public Component
{
public:
    Resistor(QString _name, int value, int _xPosition, int _yPosition, bool isVertical);
    ~Resistor();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    //getter
    virtual int getXStartPosition(void) override;
    virtual int getYStartPosition(void) override;

    static int getCount(void) {return _resistorCount;}

    void setResistance(int resistance) {_value = resistance;}

private:
    static int _resistorCount;

    QString _name;
    int _value;

};

#endif // RESISTOR_H
