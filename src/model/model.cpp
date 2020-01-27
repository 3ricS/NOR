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

void Model::addConnection(int xStart, int yStart, int xEnd, int yEnd)
{
    Component* connection = new Connection(xStart, yStart, xEnd, yEnd);
    addObject(connection);
}

void Model::clickInterpretation(QPointF position, QPointF positionEnd)
{
    //filter position to make a grid
    position.setX(position.toPoint().x() - (position.toPoint().x() % 100) + 50);
    position.setY(position.toPoint().y() - (position.toPoint().y() % 100) - 50);
    //positionEnd.setX(position.toPoint().x() - (positionEnd.toPoint().x() % 100) + 50);
    //positionEnd.setY(position.toPoint().y() - (positionEnd.toPoint().y() % 100) - 50);

    if(_mode == ResistorMode)
    {
        addResistor("R" + QString(Resistor::getResistorCount() + 1), 100, position.toPoint().x(), position.toPoint().y());
    }

    if(_mode == PowerSupplyMode)
    {
        addPowerSupply("Test", position.toPoint().x(), position.toPoint().y());
    }

    if(_mode == ConnectionMode)
    {
        addConnection(position.toPoint().x(), position.toPoint().y(), positionEnd.toPoint().x(), positionEnd.toPoint().y());
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
