#include "model/networkgraphics.h"

void NetworkGraphics::addConnection(int xStart, int yStart, int xEnd, int yEnd)
{
    Connection* connection = new Connection(xStart, yStart, xEnd, yEnd);
    _connectionList.append(connection);
    //TODO: connection als Item hinzufügen
    //addItem(connection);
    update();
}

void NetworkGraphics::clickInterpretation(QPoint position)
{
    //filter position to make a grid
    //position.setX(position.toPoint().x() - (position.toPoint().x() % 100) + 50);
    //position.setY(position.toPoint().y() - (position.toPoint().y() % 100) - 50);

    if(_mouseMode == ResistorMode)
    {
        QString name = "R" + QString::number(Resistor::getResistorCount() + 1);
        Component* resistor = new Resistor(name, 100, position.x(), position.y(), true);
        addObject(resistor);
    }

    if(_mouseMode == PowerSupplyMode)
    {
        //TODO: powersupply benennen
        Component* powerSupply = new PowerSupply("Test", position.x(), position.y(), true);
        addObject(powerSupply);
    }

    if(_mouseMode == Mouse)
    {
        //tryFindComponent(position);
    }
}

void NetworkGraphics::addObject(Component* component)
{
    _componentList.append(component);
    addItem(component);
    update();
}