#include "model/networkgraphics.h"


NetworkGraphics::NetworkGraphics() : QGraphicsScene(), _graphics(new QGraphicsScene())
{
    // die Anfangsgröße wird initialisiert
    setSceneRect(-_defaultSceneSize, -_defaultSceneSize, _defaultSceneSize, _defaultSceneSize);
    _manager = new FileManager(this);
}

void NetworkGraphics::addConnection(ComponentPort componentPortA, ComponentPort componentPortB)
{
    Connection* connection = new Connection(componentPortA, componentPortB);
    if(!_connectionList.contains(connection))
    {
        _connectionList.append(connection);
        addItem(connection);
        update();
    }
    else
    {
        qDebug() << "NetworkGraphics: Connection bereits vorhanden!";
    }

    updateCalc();
}


void NetworkGraphics::addObject(Component* component)
{
    _componentList.append(component);
    connectComponentToNeighbours(component);
    addItem(component);
    update();
}

Component* NetworkGraphics::getComponentAtPosition(QPointF gridPosition)
{
    for (Component* component : _componentList)
    {
        bool equalX = (component->getXPosition() == gridPosition.toPoint().x());
        bool equalY = (component->getYPosition() == gridPosition.toPoint().y());
        if (equalX && equalY)
        {
            return component;
        }
    }
    return nullptr;
}

bool NetworkGraphics::isThereAComponent(QPointF gridPosition)
{
    return getComponentAtPosition(gridPosition) != nullptr;
}

QString NetworkGraphics::getFileName()
{
    return _manager->getFileName();
}


void NetworkGraphics::reloadAll(void)
{
    for (Component* component : _componentList)
    {
        addItem(component);
    }
    for (Connection* connection : _connectionList)
    {
        addItem(connection);
    }
    update();
}

ComponentPort* NetworkGraphics::getComponentPortAtPosition(QPointF scenePosition)
{
    for (Component* component : _componentList)
    {
        bool hasFoundPort = component->hasPortAtPosition(scenePosition);
        if (hasFoundPort)
        {
            Component::Port port = component->getPort(scenePosition);
            ComponentPort* cp = new ComponentPort(component, port);
            return cp;
        }
    }
    return nullptr;
}

double NetworkGraphics::calculate(void)
{
    updateCalc();

    QMessageBox::about(nullptr, "Berechnung", "Der Gesamtwiderstand des Netzwerkes beträgt : " +
                                              QString::number(_calculator.getResistanceValue()) + "Ω");
    return _calculator.getResistanceValue();
}

void NetworkGraphics::save(void)
{
    _manager->save();

}

void NetworkGraphics::load(void)
{
    _manager->load();
    //reloadAll();

}

void NetworkGraphics::saveAs()
{
    _manager->saveAs();
}

void NetworkGraphics::mirrorComponent(Component* component)
{
    for (Connection* connection : _connectionList)
    {
        if (connection->hasComponent(component))
        {
            connection->changePortOfComponentPortWithComponent(component);
        }
    }
}

Component* NetworkGraphics::createNewComponent(QMouseEvent* mouseEvent, QPointF gridPosition,
                                               Component::ComponentType componentType, bool componentIsVertical)
{
    Component* createdComponent = nullptr;

    if (isThereAComponent(gridPosition))
    {
        return nullptr;
    }

    if (Component::ComponentType::Resistor == componentType)
    {

        createdComponent = addResistor("", 100, gridPosition.x(), gridPosition.y(), componentIsVertical);
    }
    else if (Component::ComponentType::PowerSupply == componentType)
    {
        createdComponent = addPowerSupply("", gridPosition.x(), gridPosition.y(), componentIsVertical);
    }

    updateCalc();
    return createdComponent;
}

Component* NetworkGraphics::addResistor(QString name, int valueResistance, int xPosition, int yPosition, bool isVertical, int id)
{
    _resistorCount++;
    if("" == name)
    {
        name = "R" + QString::number(_resistorCount);
    }
    if(0 == id)
    {
        id = _resistorCount;
    }

    Component* resistor = new Resistor(name, valueResistance, xPosition, yPosition,
                                       isVertical, id);
    addObject(resistor);

    updateCalc();
    return resistor;
}

Component* NetworkGraphics::addPowerSupply(QString name, int x, int y, bool isVertical, int id)
{
    _powerSupplyCount++;
    if("" == name)
    {
        name = "Q" + QString::number(_powerSupplyCount);
    }

    if (_powerSupplyCount <= 1)
    {
        Component* powerSupply = new PowerSupply(name, x, y,
                                                 isVertical, id);
        addObject(powerSupply);
        return powerSupply;
    }
    else
    {
        _powerSupplyCount--;
        QMessageBox::about(nullptr, "Fehleingabe", "Nur eine Spannungsquelle erlaubt");
    }
    return nullptr;
}

void NetworkGraphics::deleteComponent(Component* component)
{
    if (component != nullptr)
    {
        removeItem(component);
        _componentList.removeOne(component);

        //ResistorCount und PowerSupplyCount setzen
        if(Component::ComponentType::Resistor == component->getComponentType())
        {
            _resistorCount--;
        } else if (Component::ComponentType::PowerSupply == component->getComponentType())
        {
            _powerSupplyCount--;
        }

        //Lösche Verbindungen vom Component
        for (Connection* connection : _connectionList)
        {
            if (connection->hasComponent(component))
            {
                removeItem(connection);
                _connectionList.removeOne(connection);
                delete connection;
            }
        }

        updateCalc();
        delete component;
    }
}

void NetworkGraphics::moveComponent(Component* componentToMove, QPointF gridPosition)
{
    bool isComponentAtPosition = isThereAComponent(gridPosition);
    bool userIsMovingComponent = (nullptr != componentToMove);
    if (!userIsMovingComponent || isComponentAtPosition)
    {
        return;
    }

    componentToMove->setPosition(gridPosition);

    connectComponentToNeighbours(componentToMove);
}

void NetworkGraphics::connectComponentToNeighbours(Component* componentToConnectWithNeighbours)
{
    QPointF portPositionAOfComponent = componentToConnectWithNeighbours->getPortPosition(Component::Port::A);
    QPointF portPositionBOfComponent = componentToConnectWithNeighbours->getPortPosition(Component::Port::B);

    for (Component* anotherComponent : _componentList)
    {
        if (anotherComponent != componentToConnectWithNeighbours)
        {
            QPointF portPositionAOfanotherComponent = anotherComponent->getPortPosition(Component::Port::A);
            QPointF portPositionBOfanotherComponent = anotherComponent->getPortPosition(Component::Port::B);

            bool equalPortAtoA = (portPositionAOfComponent == portPositionAOfanotherComponent);
            bool equalPortAtoB = (portPositionAOfComponent == portPositionBOfanotherComponent);
            bool equalPortBtoA = (portPositionBOfComponent == portPositionAOfanotherComponent);
            bool equalPortBtoB = (portPositionBOfComponent == portPositionBOfanotherComponent);

            ComponentPort componentPortSelf(nullptr, Component::Port::null);
            ComponentPort componentPortOther(nullptr, Component::Port::null);

            if (equalPortAtoA)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::A);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::A);
            }
            else if (equalPortAtoB)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::A);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::B);
            }
            else if (equalPortBtoA)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::B);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::A);
            }
            else if (equalPortBtoB)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::B);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::B);
            }

            bool foundEqualPort = (equalPortAtoA || equalPortAtoB || equalPortBtoA || equalPortBtoB);
            if(foundEqualPort)
            {
                addConnection(componentPortSelf, componentPortOther);
            }

        }
    }
}

void NetworkGraphics::turnComponentLeft(Component* componentToTurn)
{
    // 3 mal rechts ist einmal links
    for(int i = 0; i < 3; i++)
    {
        turnComponentRight(componentToTurn);
    }
}

void NetworkGraphics::turnComponentRight(Component* componentToTurn)
{
    switch (componentToTurn->getOrientation())
    {
        case Component::Orientation::left:
        {
            componentToTurn->setOrientation(Component::Orientation::top);
            //mirrorComponent(componentToTurn);
        }
        break;
        case Component::Orientation::top:
        {
            componentToTurn->setOrientation(Component::Orientation::right);
            mirrorComponent(componentToTurn);
        }
            break;
        case Component::Orientation::right:
        {
            componentToTurn->setOrientation(Component::Orientation::bottom);
        }
        break;
        case Component::Orientation::bottom:
        {
            componentToTurn->setOrientation(Component::Orientation::left);
            mirrorComponent(componentToTurn);
        }
        break;
    }
}

void NetworkGraphics::setOrientationOfComponent(Component* componentToTurn, Component::Orientation orientation)
{
    while(componentToTurn->getOrientation() != orientation)
    {
        turnComponentRight(componentToTurn);
    }
}

void NetworkGraphics::updateCalc(void)
{
    _calculator.setLists(_connectionList, _componentList);
    _calculator.calculate();
    qDebug() << "Neuer Widerstandswert: " << _calculator.getResistanceValue();
    //TODO: neuen Widerstandswert anzeigen
}
