#include "model.h"

#include <qpoint.h>

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

void Model::clickInterpretation(QPointF position)
{
    if(_mode == ResistorMode)
    {
        addResistor("R" + QString(Resistor::getResistorCount() + 1), 100, position.toPoint().x(), position.toPoint().y());
    }

    if(_mode == PowerSupplyMode)
    {
        addPowerSupply("Test", position.toPoint().x(), position.toPoint().y());
    }

    if(_mode == Mouse)
    {
        tryFindComponent(position);
    }
}

//Es wird versucht das angeklickte Objekt ausfindig zu machen -- Funktioniert noch nicht
void Model::tryFindComponent(QPointF position)
{
    for(Component* c : _componentList)
    {
        if((c->getXPosition() + 100 > position.rx() && c->getXPosition() - 100 < position.rx()) && (c->getYPosition() + 100 < position.ry() && c->getYPosition() - 100 < position.ry()))
        {
            qDebug() << "Gefunden!";
        }
    }

}

void Model::addObject(Component* component)
{
    _componentList.append(component);

    //the model has changed now
    emit modelChanged();
}
