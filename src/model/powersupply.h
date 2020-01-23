#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

#include "component.h"

#include <QString>

class PowerSupply : public Component
{
public:
    PowerSupply();

    void createPowerSupply();

private:
    QString _name;

    // Component interface
signals:
    virtual void draw() override;
    virtual void remove() override;
};

#endif // POWERSUPPLY_H
