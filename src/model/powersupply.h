#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

#include <QString>

#include <model/component.h>

class PowerSupply : public Component
{
public:
    PowerSupply();

private:
    QString _name;
};

#endif // POWERSUPPLY_H
