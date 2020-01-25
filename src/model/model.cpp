#include "model.h"

Model::Model(QObject *parent) : QObject(parent)
{

}

void Model::addResistor(QString name, int value, int x, int y)
{
    Component* resistor = new Resistor(name, value, x, y);
    addObject(resistor);
}

void Model::addPowerSupply(QString name, int x, int y)
{
    Component* powersupply = new PowerSupply(name, x, y);
    addObject(powersupply);
}

void Model::addObject(Component* component)
{
    _componentList.append(component);

    //the model has changed now
    emit modelChanged();
}
