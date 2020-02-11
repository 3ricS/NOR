#ifndef RESISTOR_H
#define RESISTOR_H

#include <model/component.h>

class Resistor : public Component
{
public:
    Resistor(QString _name, int valueResistance, int _xPosition, int _yPosition, bool isVertical);

    //Methoden
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


    //getter

    //Klassenmethoden

    //setter
    void setResistance(int resistance) {_value = resistance;}


private:

    //Klassenvariablen

    //Methoden
    virtual void paintInformations(QPainter* painter) override;
    void paintOrientationSensitiv(QPainter* painter, int xPosText, int yPosText, int xPosValue, int yPosValue);
    //Variablen

};

#endif // RESISTOR_H
