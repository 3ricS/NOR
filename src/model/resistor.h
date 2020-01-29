#ifndef RESISTOR_H
#define RESISTOR_H

#include <model/component.h>

class Resistor : public Component
{
public:
    Resistor(QString _name, int value, int _xPosition, int _yPosition, bool isVertical);
    ~Resistor();

    virtual void show(QGraphicsScene *scene) override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    //getter
    virtual int getXStartPosition(void) override;
    virtual int getYStartPosition(void) override;

    static int getResistorCount(void) {return _resistorCount;}
    int getWidth(void) const {return _width;}
    int getHeight(void) const {return _height;}

    void setResistance(int resistance) {_value = resistance;}

private:
    static int _resistorCount;
    static constexpr int _height = 100;
    static constexpr int _width = 40;

    QString _name;
    int _value;

    // QGraphicsItem interface

};

#endif // RESISTOR_H
