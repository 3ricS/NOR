#ifndef RESISTOR_H
#define RESISTOR_H

#include <model/component.h>

class Resistor : public Component
{
public:
    Resistor(QString _name, int value, int _xPosition, int _yPosition, bool isVertical);
    ~Resistor();

    //Methoden
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


    //getter
    virtual int getXStartPosition(void) override;
    virtual int getYStartPosition(void) override;
    virtual int getValue() override{return _value;}
    virtual QString getName() override{return _name;}

    //Klassenmethoden
    static int getCount(void) {return _resistorCount;}

    //setter
    void setResistance(int resistance) {_value = resistance;}
    virtual void setName(QString name) override{_name = name;}

private:

    //Klassenvariablen
    static int _resistorCount;

    //Methoden
    virtual void paintInformations(QPainter* painter) override;

    //Variablen
    int _value;
    QString _name;

};

#endif // RESISTOR_H
