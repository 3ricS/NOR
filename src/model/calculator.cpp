#include "calculator.h"

Calculator::Calculator(QList<Connection*> connections, QList<Component*> components) :
        _components(components), _connections(connections)
{
}
//TODO: Fehler abfangen, Algo festigen ggf. nochmal kompl. neu mit neuem Ansatz, parallel

void Calculator::calculate()
{
    double actualImpedanz = 0;
    Component* firstComponent = nullptr;
    for (Connection* connection : _connections)
    {
        //Suche nach dem ersten Widerstand
        if((connection->getComponentA()->getComponentType() == Component::PowerSupply) ||
           (connection->getComponentB()->getComponentType() == Component::PowerSupply))
        {
            if(connection->getComponentA()->getComponentType() == Component::PowerSupply)
            {
                firstComponent = connection->getComponentB();
            }
            else
            {
                firstComponent = connection->getComponentA();
            }
            //_connections.removeOne(connection);
            break;
        }
    }
    //rowAnalysis(firstComponent, actualImpedanz);
    //_resistanceValue = actualImpedanz;

    QList<ComponentPort> connectedComponents;
    connectedComponents = findConnectedComponents(ComponentPort(firstComponent, Component::Port::A),
                                                  connectedComponents);
}

void Calculator::rowAnalysis(Component* comp, double& actualImpedanz, Component* lastComponent)
{
    int count = 0;
    Component* nextComponent = nullptr;
    for (Connection* connection : _connections)
    {
        if(connection->getComponentA() == comp && connection->getComponentB() != lastComponent)
        {
            count++;
            nextComponent = connection->getComponentB();
        }
        else if(connection->getComponentB() == comp && connection->getComponentA() != lastComponent)
        {
            count++;
            nextComponent = connection->getComponentA();
        }
    }
    if(count == 1)
    {
        actualImpedanz += comp->getValue();
        rowAnalysis(nextComponent, actualImpedanz, comp);
    }
    qDebug() << actualImpedanz;
}

QList<ComponentPort>
Calculator::findConnectedComponents(ComponentPort componentPort, QList<ComponentPort>& connectedComponents)
{
    for (Connection* connection : _connections)
    {
        Component* foundComponent;
        Component::Port foundPort;
        bool found = false;
        if(componentPort.getComponent() == connection->getComponentB())
        {
            foundComponent = connection->getComponentA();
            foundPort = connection->getPortA();
            found = true;
        }
        else if(componentPort.getComponent() == connection->getComponentA())
        {
            foundComponent = connection->getComponentB();
            foundPort = connection->getPortB();
            found = true;
        }
        if(found)
        {
            ComponentPort foundComponentPort = ComponentPort(foundComponent, foundPort);
            if(!connectedComponents.contains(foundComponentPort))
            {
                connectedComponents.append(foundComponentPort);
                QList<ComponentPort> connectedComponentsOfFoundComponent = findConnectedComponents(foundComponentPort,
                                                                                                   connectedComponents);
            }
        }
    }
    return connectedComponents;
}
