#include "model/networkgraphics.h"


NetworkGraphics::NetworkGraphics() : QGraphicsScene(), _graphics(new QGraphicsScene())
{
    // die Anfangsgröße wird initialisiert
    setSceneRect(-_defaultSceneSize, -_defaultSceneSize, _defaultSceneSize, _defaultSceneSize);
}

void NetworkGraphics::deleteItem()
{
    if (_selectedComponent != nullptr)
    {
        removeItem(_selectedRect);
        _componentList.removeOne(_selectedComponent);

        //Lösche Verbindungen vom Component
        for (Connection* connection : _connectionList)
        {
            if (connection->hasComponent(_selectedComponent))
            {
                removeItem(connection);
                _connectionList.removeOne(connection);
                delete connection;
            }
        }

        delete _selectedComponent;
        _selectedComponent = nullptr;
    }
}

void NetworkGraphics::addConnection(ComponentPort componentPortA, ComponentPort componentPortB)
{
    Connection* connection = new Connection(componentPortA, componentPortB);
    _connectionList.append(connection);
    addItem(connection);
    update();
}


void NetworkGraphics::addObject(Component* component)
{
    _selectedComponent = component;
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

void NetworkGraphics::calculate(void)
{
    Calculator calculator = Calculator(_connectionList, _componentList);
    calculator.calculate();
    QMessageBox::about(nullptr, "Berechnung", "Der Gesamtwiderstand des Netzwerkes beträgt : " +
                                              QString::number(calculator.getResistanceValue()) + "Ω");
}

void NetworkGraphics::save(void)
{
    FileManager manager(_componentList, _connectionList);
    manager.saving();
}

void NetworkGraphics::load(void)
{
    FileManager manager(_componentList, _connectionList);
    manager.loading();
    reloadAll();
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
        QString name = "R" + QString::number(Resistor::getCount() + 1);
        Component* resistor = new Resistor(name, 100, gridPosition.x(), gridPosition.y(),
                                           componentIsVertical);
        createdComponent = resistor;
        addObject(resistor);
    }
    else if (Component::ComponentType::PowerSupply == componentType)
    {
        if (PowerSupply::getCount() < 1)
        {
            QString name = "Q" + QString::number(PowerSupply::getCount() + 1);
            Component* powerSupply = new PowerSupply(name, gridPosition.x(), gridPosition.y(),
                                                     componentIsVertical);
            createdComponent = powerSupply;
            addObject(powerSupply);
        }
        else
        {
            QMessageBox::about(nullptr, "Fehleingabe", "Nur eine Spannungsquelle erlaubt");
        }
    }

    return createdComponent;
}