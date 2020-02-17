#include "model/networkgraphics.h"


NetworkGraphics::NetworkGraphics() : QGraphicsScene(), _graphics(new QGraphicsScene())
{
    // die Anfangsgröße wird initialisiert
    setSceneRect(-_defaultSceneSize, -_defaultSceneSize, _defaultSceneSize, _defaultSceneSize);
}

void NetworkGraphics::addConnection(ComponentPort componentPortA, ComponentPort componentPortB)
{
    Connection* connection = new Connection(componentPortA, componentPortB);
    if(!_connectionList.contains(connection))
    {
        _connectionList.append(connection);
        addItem(connection);
        update();
    } else {
        qDebug() << "NetworkGraphics: Connection bereits vorhanden!";
    }
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
    Calculator calculator = Calculator(_connectionList, _componentList);
    calculator.calculate();

    QMessageBox::about(nullptr, "Berechnung", "Der Gesamtwiderstand des Netzwerkes beträgt : " +
                                              QString::number(calculator.getResistanceValue()) + "Ω");
    return calculator.getResistanceValue();
}

void NetworkGraphics::save(void)
{
    if(_manager == nullptr)
    {
        _manager = new FileManager(_componentList, _connectionList);
    }
    else
    {
        _manager->setProperties(_componentList, _connectionList);
    }

    _manager->saving();
}

void NetworkGraphics::load(void)
{
    if(_manager == nullptr)
    {
        _manager = new FileManager(_componentList, _connectionList);
    }
    else
    {
        _manager->setProperties(_componentList, _connectionList);
    }

    _manager->loading();
    reloadAll();
}

void NetworkGraphics::saveAs()
{
    if(_manager == nullptr)
    {
        _manager = new FileManager(_componentList, _connectionList);
    }
    else
    {
        _manager->setProperties(_componentList, _connectionList);
    }

    _manager->savingUnder();
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

        createdComponent = addResistor(100, gridPosition.x(), gridPosition.y(), componentIsVertical);
    }
    else if (Component::ComponentType::PowerSupply == componentType)
    {
        createdComponent = addPowerSupply(gridPosition.x(), gridPosition.y(), componentIsVertical);
    }

    return createdComponent;
}

Component* NetworkGraphics::addResistor(int valueResistance, int xPosition, int yPosition, bool isVertical)
{
    _resistorCount++;
    QString name = "R" + QString::number(_resistorCount);
    Component* resistor = new Resistor(name, valueResistance, xPosition, yPosition,
                                       isVertical);
    addObject(resistor);
    return resistor;
}

Component* NetworkGraphics::addPowerSupply(int x, int y, bool isVertical)
{
    _powerSupplyCount++;
    if (_powerSupplyCount <= 1)
    {
        QString name = "Q" + QString::number(_powerSupplyCount);
        Component* powerSupply = new PowerSupply(name, x, y,
                                                 isVertical);
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
            }else if (equalPortBtoA)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::B);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::A);
            }else if (equalPortBtoB)
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
