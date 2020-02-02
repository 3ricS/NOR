#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

#include <model/component.h>

class PowerSupply : public Component
{
public:
    PowerSupply(QString name, int x, int y, bool isVertical, int valueVoltage);

    //Methoden
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


    //getter

    //setter

    //Klassenmethoden
    static int getCount(void) {return _powerSupplyCount;}

private:

    //Klassenvariablen
    static int _powerSupplyCount;

    //Methodenn
    virtual void paintInformations(QPainter* painter) override;

    //Variablen
};

#endif // POWERSUPPLY_H
