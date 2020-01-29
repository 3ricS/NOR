#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

#include <model/component.h>

class PowerSupply : public Component
{
public:
    PowerSupply(QString name, int _xPosition, int _yPosition, bool isVertical);

    //Methoden
    virtual void show(QGraphicsScene *scene) override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    //getter
    virtual int getXStartPosition(void) override;
    virtual int getYStartPosition(void) override;

    int getWidth(void) const {return _width;}
    int getHeight(void) const {return _height;}

private:
    QString _name;
    static constexpr int _height = 120;
    static constexpr int _width = 60;

};

#endif // POWERSUPPLY_H
