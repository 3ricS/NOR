#ifndef RESISTOR_H
#define RESISTOR_H

#include <model/component.h>

class Resistor : public Component
{
public:
    Resistor(QString _name, int valueResistance, int _xPosition, int _yPosition, bool isVertical);
    ~Resistor();

    //Methoden
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


    //getter

    //Klassenmethoden
    static int getCount(void) {return _resistorCount;}

    //setter
    void setResistance(int resistance) {_value = resistance;}


private:

    //Klassenvariablen
    static int _resistorCount;

    //Methoden
    virtual void paintInformations(QPainter* painter) override;

    //Variablen

};

#endif // RESISTOR_H
