#include "model/networkgraphics.h"


NetworkGraphics::NetworkGraphics() : QGraphicsScene(), _graphics(new QGraphicsScene())
{
    // die Anfangsgröße wird initialisiert
    setSceneRect(-_defaultSceneSize, -_defaultSceneSize, _defaultSceneSize, _defaultSceneSize);
}

void NetworkGraphics::mouseReleaseInterpretation(QPointF position)
{
    pointToGrid(&position);
    //filter position to make a grid
    //position.setX(position.toPoint().x() - (position.toPoint().x() % 100) + 50);
    //position.setY(position.toPoint().y() - (position.toPoint().y() % 100) - 50);

    switch (_mouseMode)
    {
    case MouseMode::ResistorMode:
    {
        if(isThereAComponent(&position))
        {
            return;
        }

        QString name = "R" + QString::number(Resistor::getCount() + 1);
        Component* resistor = new Resistor(name, 756, position.x(), position.y(), true);
        addObject(resistor);
    }
    break;
    case MouseMode::PowerSupplyMode:
    {
        if (isThereAComponent(&position))
        {
            return;
        }

        QString name = "Q" + QString::number(PowerSupply::getCount() + 1);
        Component* powerSupply = new PowerSupply(name, position.x(), position.y(), false, 100);
        addObject(powerSupply);
    }
    break;
    case MouseMode::ConnectionMode:
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

void NetworkGraphics::mouseDoublePressInterpretation(QPointF position)
{
    if(_mouseMode == Mouse)
    {
        pointToGrid(&position);

        if(isThereAComponent(&position))
        {
            for(Component* component : _componentList)
            {
                if((component->getXPosition() == position.toPoint().x()) && (component->getYPosition() == position.toPoint().y()))
                {
                    _editingView = new EditView(component);
                    _editingView->show();
                }
            }
        }
    }

}

void NetworkGraphics::mouseMoveInterpretation(QPointF position)
{
    pointToGrid(&position);
    //TODO: Zoomfaktor einfügen
    int positionX = position.toPoint().x();
    int positionY = position.toPoint().y();
    QColor highlightColor = QColor("#ffcccc");
    QGraphicsItem* highlightedRect = addRect(positionX - 50, positionY - 50, 100, 100, Qt::NoPen, highlightColor);
    removeItem(_previousRect);
    _previousRect = highlightedRect;
}

void NetworkGraphics::addConnection(int xStart, int yStart, int xEnd, int yEnd)
{
    Connection* connection = new Connection(xStart, yStart, xEnd, yEnd);
    _connectionList.append(connection);
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
        bool equalX = (component->getXPosition() == position->toPoint().x());
        bool equalY = (component->getYPosition() == position->toPoint().y());
        if(equalX && equalY)
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
