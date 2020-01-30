#include "model/networkgraphics.h"


NetworkGraphics::NetworkGraphics() : QGraphicsScene(), _graphics(new QGraphicsScene())
{
    setSceneRect(-_defaultSceneSize, -_defaultSceneSize, _defaultSceneSize, _defaultSceneSize);
}

void NetworkGraphics::mouseReleaseInterpretation(QPointF position)
{
    pointToGrid(&position);
    //filter position to make a grid
    //position.setX(position.toPoint().x() - (position.toPoint().x() % 100) + 50);
    //position.setY(position.toPoint().y() - (position.toPoint().y() % 100) - 50);

    if(isThereAComponent(&position) == true)
    {
        return;
    }
    switch (_mouseMode)
    {
        case MouseMode::ResistorMode:
        {
            QString name = "R" + QString::number(Resistor::getCount() + 1);
            Component* resistor = new Resistor(name, 100, position.x(), position.y(), true);
            addObject(resistor);
        }
            break;
        case MouseMode::PowerSupplyMode:
        {
            QString name = "Q" + QString::number(PowerSupply::getCount() + 1);
            Component* powerSupply = new PowerSupply(name, position.x(), position.y(), true);
            addObject(powerSupply);
        }
            break;
        case ConnectionMode:
        {
            addConnection(_connectionPointStart.x(), _connectionPointStart.y(), position.x(), position.y());
        }
            break;
        default:
            break;
    }
}

void NetworkGraphics::mousePressInterpretation(QPointF position)
{
    pointToGrid(&position);

    if (_mouseMode != ConnectionMode)
    {
        _connectionStarted = false;
    }

    switch (_mouseMode)
    {
        case ConnectionMode:
            _connectionStarted = true;
            _connectionPointStart = position;
            break;
    }
}

void NetworkGraphics::addConnection(int xStart, int yStart, int xEnd, int yEnd)
{
    Connection* connection = new Connection(xStart, yStart, xEnd, yEnd);
    _connectionList.append(connection);
    //TODO: connection als Item hinzufügen
    addItem(connection);
    update();
}

void NetworkGraphics::addObject(Component* component)
{
    _componentList.append(component);
    addItem(component);
    update();
}

bool NetworkGraphics::isThereAComponent(QPointF* position)
{
    for(const Component* component : _componentList )
    {
        if((component->getXPosition() == position->toPoint().x()) && (component->getYPosition() == position->toPoint().y()))
        {
            return true;
        }
    }
    return false;
}

void NetworkGraphics::pointToGrid(QPointF* position)
{
    position->setX(position->toPoint().x() / 100 * 100 - 50);
    position->setY(position->toPoint().y() / 100 * 100 - 50);
}
