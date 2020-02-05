#include "calculator.h"

Calculator::Calculator(QList<Connection*> connections, QList<Component*> components) :
        _components(components), _connections(connections)
{
}
//TODO: Fehler abfangen, Algo festigen ggf. nochmal kompl. neu mit neuem Ansatz, parallel

void Calculator::calculate()
{
    double actualImpedanz = 0;
    QList<ComponentPort> foundComponents;
    for (Connection* connection : _connections)
    {
        //Suche nach den ersten Widerständen
        if(((connection->getComponentPortA().getComponent()->getComponentType() == Component::PowerSupply) &&
                connection->getComponentPortA().getPort() == Component::A ) ||
           ((connection->getComponentPortB().getComponent()->getComponentType() == Component::PowerSupply && connection->getComponentPortB().getPort() == Component::A)))
        {
            if(connection->getComponentPortA().getComponent()->getComponentType() == Component::PowerSupply)
            {
                foundComponents.append(connection->getComponentPortB());
                _connections.removeOne(connection);
            }
            if(connection->getComponentPortB().getComponent()->getComponentType() == Component::PowerSupply)
            {
                foundComponents.append(connection->getComponentPortA());
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
        for(ComponentPort actualComponentPort : foundComponents)
        {
            //Möchte vom entgegengesetzten Port ausgehen
            actualComponentPort.invertPort();
            QList<ComponentPort> newFoundComps;

            //Hinzufügen der gefundenen Objekte vom entgegengesetzten Port
            for(Connection* c : _connections)
            {
                if(actualComponentPort == c->getComponentPortA() &&
                        c->getComponentPortA().getComponent()->getComponentType() != Component::ComponentType::PowerSupply)
                {
                    newFoundComps.append(c->getComponentPortA());
                }
                else if (actualComponentPort == c->getComponentPortB() &&
                        c->getComponentPortB().getComponent()->getComponentType() != Component::ComponentType::PowerSupply) {
                    newFoundComps.append(c->getComponentPortB());
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
        _resistanceValue = actualImpedanz;
    }



   // QList<ComponentPort> connectedComponents;
   // connectedComponents = findConnectedComponents(ComponentPort(firstComponent, Component::Port::A),
                                                 // connectedComponents);
}

void Calculator::rowAnalysis(Component* comp, double& actualImpedanz, Component* lastComponent)
{
    int count = 0;
    Component* nextComponent = nullptr;
    for (Connection* connection : _connections)
    {
        if(connection->getComponentPortA().getComponent() == comp && connection->getComponentPortB().getComponent() != lastComponent)
        {
            count++;
            nextComponent = connection->getComponentPortB().getComponent();
        }
        else if(connection->getComponentPortB().getComponent() == comp && connection->getComponentPortA().getComponent() != lastComponent)
        {
            count++;
            nextComponent = connection->getComponentPortA().getComponent();
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
        ComponentPort foundComponentPort = ComponentPort(nullptr, Component::Port::null);

        bool found = false;
        if(componentPort.getComponent() == connection->getComponentPortB().getComponent())
        {
            Component* foundComponent = connection->getComponentPortA().getComponent();
            Component::Port foundPort = connection->getComponentPortA().getPort();
            foundComponentPort = ComponentPort(foundComponent, foundPort);
            found = true;
        }
        else if(componentPort.getComponent() == connection->getComponentPortA().getComponent())
        {
            Component* foundComponent = connection->getComponentPortB().getComponent();
            Component::Port foundPort = connection->getComponentPortB().getPort();
            foundComponentPort = ComponentPort(foundComponent, foundPort);
            found = true;
        }

        if(found)
        {
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
