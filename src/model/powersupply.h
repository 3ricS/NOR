#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

#include <QString>

#include <model/component.h>

class PowerSupply : public Component
{
public:
    PowerSupply(QString name, int x, int y);

    //getter
    virtual int getXStartPosition() override;
    virtual int getYStartPosition() override;

private:
    QString _name;

};

#endif // POWERSUPPLY_H
