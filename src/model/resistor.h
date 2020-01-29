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
    virtual int getXStartPosition() override;
    virtual int getYStartPosition() override;

    static int getResistorCount() {return _resistorCount;}
    int getWidth() {return _width;}
    int getHeight() {return _height;}

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
