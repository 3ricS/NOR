#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

#include <model/component.h>

class PowerSupply : public Component
{
public:
    PowerSupply(QString name, int _xPosition, int _yPosition, bool isVertical);

    //Methoden
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


    //getter
    virtual int getXStartPosition(void) override;
    virtual int getYStartPosition(void) override;
    virtual int getValue() override;
    virtual QString getName() override{return _name;}

    //setter
    virtual void setName(QString name) override{_name = name;}

    //Klassenmethoden
    static int getCount() {return _powerSupplyCount;}

private:

    //Klassenvariablen
    static int _powerSupplyCount;

    //Methodenn
    virtual void paintInformations(QPainter* painter) override;

    //Variablen
    QString _name;
};

#endif // POWERSUPPLY_H
