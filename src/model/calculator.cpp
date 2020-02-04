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
    QList<ComponentPort> foundComponents;
    for (Connection* connection : _connections)
    {
        //Suche nach den ersten Widerständen
        if(((connection->getComponentA()->getComponentType() == Component::PowerSupply) && connection->getPortA() == Component::A )||
           ((connection->getComponentB()->getComponentType() == Component::PowerSupply && connection->getPortB() == Component::A)))
        {
            if(connection->getComponentA()->getComponentType() == Component::PowerSupply)
            {
                foundComponents.append(ComponentPort(connection->getComponentB(),connection->getPortB()));
                _connections.removeOne(connection);
            }
            if(connection->getComponentB()->getComponentType() == Component::PowerSupply)
            {
                foundComponents.append(ComponentPort(connection->getComponentA(),connection->getPortA()));
                _connections.removeOne(connection);
            }

        }
    }
    //Parallelschaltung wurde erkannt
    if(foundComponents.count() >= 2)
    {
        //ob gefunden wurde
        bool gefunden = false;
        //Analyse der einzelnen Pfade des vorher gefundenen
        for(ComponentPort c : foundComponents)
        {
            //Möchte vom entgegengesetzten Port ausgehen
            Component* actualComp = c.getComponent();
            Component::Port aPort = c.getOppisitePort();
            QList<ComponentPort> newFoundComps;

            //Hinzufügen der gefundenen Objekte vom entgegengesetzten Port
            for(Connection* c : _connections)
            {
                if(actualComp == c->getComponentA() && c->getPortA() == aPort && c->getComponentA()->getComponentType() != Component::ComponentType::PowerSupply)
                {
                    newFoundComps.append(ComponentPort(c->getComponentA(),c->getPortA()));
                }
                else if (actualComp == c->getComponentB() && c->getPortB() == aPort && c->getComponentB()->getComponentType() != Component::ComponentType::PowerSupply) {
                    newFoundComps.append(ComponentPort(c->getComponentB(),c->getPortB()));
                }
            }
            //Wenn soviele Componenten gefunden wurden wie in der Liste waren können wir sagen Schaltung wurde geschlossen
            int i = 0;
            for(ComponentPort cp : foundComponents)
            {
                for(ComponentPort ck : newFoundComps)
                {
                    if(cp.getComponent() == ck.getComponent())
                    {
                        qDebug() << ck.getComponent()->getName();
                        i++;
                        break;

                    }
                }
            }
            if(i == foundComponents.count())
            {
                gefunden = true;
            }
            if(gefunden)
            {
                break;
            }
        }
        //Berechnung der Werte
        double zaehler = 1;
        for(int i = 0; i < foundComponents.count(); i++)
        {
            zaehler *= foundComponents[i].getComponent()->getValue();
        }
        double nenner = 0;
        for(int i = 0; i < foundComponents.count(); i++)
        {
            nenner += foundComponents[i].getComponent()->getValue();
        }
        _resistanceValue = zaehler / nenner;
    }
    if(foundComponents.count() == 1)
    {
        rowAnalysis(foundComponents[0].getComponent(), actualImpedanz);
    }

    _resistanceValue = actualImpedanz;

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
