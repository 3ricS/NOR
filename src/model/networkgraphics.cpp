#include "model/networkgraphics.h"


//TODO: Einbinden in NetworkView
//Mouse interaction und Entscheidung je nachdem in welchem Modus man ist
void NetworkGraphics::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //Herausfinden der Position die geklicked wurde
    QPointF position = event->scenePos();

    //Ansprechen des Model("Hier wurde etwas geklickt an der und der Position was muss ich tun")
    clickInterpretation(position);
}


void NetworkGraphics::addResistor(QString name, int value, int x, int y, bool isVertical)
{
    Component* resistor = new Resistor(name, value, x, y, isVertical);
    _componentList.append(resistor);
}

void NetworkGraphics::addPowerSupply(QString name, int x, int y, bool isVertical)
{
    Component* powersupply = new PowerSupply(name, x, y, isVertical);
    _componentList.append(powersupply);
    addItem(powersupply);
    update();
}

void NetworkGraphics::addConnection(int xStart, int yStart, int xEnd, int yEnd)
{
    Connection* connection = new Connection(xStart, yStart, xEnd, yEnd);
    _connectionList.append(connection);
}

void NetworkGraphics::clickInterpretation(QPointF position)
{
    //filter position to make a grid
    position.setX(position.toPoint().x() - (position.toPoint().x() % 100) + 50);
    position.setY(position.toPoint().y() - (position.toPoint().y() % 100) - 50);
    //positionEnd.setX(position.toPoint().x() - (positionEnd.toPoint().x() % 100) + 50);
    //positionEnd.setY(position.toPoint().y() - (positionEnd.toPoint().y() % 100) - 50);

    if(_mouseMode == ResistorMode)
    {
        addResistor("R" + QString(Resistor::getResistorCount() + 1), 100, position.toPoint().x(), position.toPoint().y(), true);
    }

    if(_mouseMode == PowerSupplyMode)
    {
        addPowerSupply("Test", position.toPoint().x(), position.toPoint().y(), true);
    }

    if(_mouseMode == Mouse)
    {
        //tryFindComponent(position);
    }
}

