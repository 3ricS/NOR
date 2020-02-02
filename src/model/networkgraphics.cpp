#include "model/networkgraphics.h"


NetworkGraphics::NetworkGraphics() : QGraphicsScene(), _graphics(new QGraphicsScene())
{
    // die Anfangsgröße wird initialisiert
    setSceneRect(-_defaultSceneSize, -_defaultSceneSize, _defaultSceneSize, _defaultSceneSize);
}

void NetworkGraphics::mouseReleaseInterpretation(QPointF position)
{
    switch (_mouseMode)
    {
        case MouseMode::ResistorMode:
        {
            pointToGrid(&position);
            if(isThereAComponent(position))
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
            pointToGrid(&position);
            if(isThereAComponent(position))
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
            if(_connectionStarted)
            {
                bool hasFoundPort;
                Component* connectionComponentEnd = getComponentWithPortAtPosition(position, hasFoundPort);
                if(hasFoundPort)
                {
                    Component::Port connectionComponentEndPort = connectionComponentEnd->getPort(position);
                    addConnection(_connectionComponentStart, _connectionComponentStartPort, connectionComponentEnd,
                                  connectionComponentEndPort);
                }
            }
        }
            break;
        case MouseMode::SelectionMode:
        {
            pointToGrid(&position);
            bool hasSelectedComponentToMove = (_selectedComponentToMove != nullptr);
            if(hasSelectedComponentToMove)
            {
                _selectedComponentToMove->setPosition(position);
                update();
            }
        }
        break;
        default:
            break;
    }
}

void NetworkGraphics::mousePressInterpretation(QPointF position)
{
    if(_mouseMode != ConnectionMode)
    {
        _connectionStarted = false;
    }

    switch (_mouseMode)
    {
        case ConnectionMode:
        {
            bool hasFoundPort;
            Component* foundComponent = getComponentWithPortAtPosition(position, hasFoundPort);
            if(hasFoundPort)
            {
                _connectionStarted = true;
                _connectionComponentStart = foundComponent;
                _connectionComponentStartPort = foundComponent->getPort(position);
            } else
            {
                _connectionStarted = false;
            }
        }
            break;
        case SelectionMode:
        {
            pointToGrid(&position);
            _selectedComponentToMove = getComponentAtPosition(position);
        }
            break;
    }
}

void NetworkGraphics::mouseDoublePressInterpretation(QPointF position)
{
    if(_mouseMode == SelectionMode)
    {
        pointToGrid(&position);
        Component* foundComponent = getComponentAtPosition(position);
        if(foundComponent != nullptr)
        {
            _editingView = new EditView(foundComponent);
            _editingView->show();
        }
    }

}

void NetworkGraphics::mouseMoveInterpretation(QPointF position)
{
    if((_mouseMode == ResistorMode) || (_mouseMode == PowerSupplyMode))
    {
        pointToGrid(&position);
        //TODO: Zoomfaktor einfügen
        int positionX = position.toPoint().x();
        int positionY = position.toPoint().y();
        QColor highlightColor = QColor(136, 136, 136, 55);  //3 mal 136 ist grau und 55 ist die Transparenz
        QGraphicsItem* highlightedRect = addRect(positionX - 50, positionY - 50, 100, 100, Qt::NoPen, highlightColor);
        removeItem(_previousRect);
        _previousRect = highlightedRect;

        update();
    }
    if((_mouseMode == ConnectionMode) || (_mouseMode == SelectionMode))
    {
        removeItem(_previousRect);
    }

}

void NetworkGraphics::addConnection(Component* componentA, Component::Port componentAPort, Component* componentB,
                                    Component::Port componentBPort)
{
    Connection* connection = new Connection(componentA, componentAPort, componentB, componentBPort);
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

Component* NetworkGraphics::getComponentAtPosition(QPointF gridPosition)
{
    for (Component* component : _componentList)
    {
        bool equalX = (component->getXPosition() == gridPosition.toPoint().x());
        bool equalY = (component->getYPosition() == gridPosition.toPoint().y());
        if(equalX && equalY)
        {
            return component;
        }
    }
    return nullptr;
}

bool NetworkGraphics::isThereAComponent(QPointF position)
{
    return getComponentAtPosition(position) != nullptr;
}

void NetworkGraphics::pointToGrid(QPointF* position)
{
    position->setX(position->toPoint().x() / 100 * 100 - 50);
    position->setY(position->toPoint().y() / 100 * 100 - 50);
}

Component* NetworkGraphics::getComponentWithPortAtPosition(QPointF position, bool& hasFoundPort)
{
    for (Component* component : _componentList)
    {
        hasFoundPort = component->hasPortAtPosition(position);
        if(hasFoundPort)
        {
            //Component::Port foundPort = component->getPort(position);
            //QPointF portPosition = component->getComponentWithPortAtPosition(foundPort);
            return component;
        }
    }
    hasFoundPort = false;
    return nullptr;
}
