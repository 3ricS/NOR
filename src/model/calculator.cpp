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
        //Suche nach den ersten Widerständen und entfernen der Verbindungen zum Port A der PowerSupply
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

    lookingForNeighbours(foundComponents);

    //Parallelschaltung wurde erkannt
    if(foundComponents.count() >= 2)
    {
        parallelAnalysis(foundComponents, actualImpedanz);
    }
    if(foundComponents.count() == 1)
    {
        rowAnalysis(foundComponents[0].getComponent(), actualImpedanz);

    }

    _resistanceValue = actualImpedanz;
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

void Calculator::parallelAnalysis(QList<ComponentPort>& foundComponents, double& actualImpedanz)
{
    //ob gefunden wurde
   // bool isEndParallel = false;

    //Analyse der einzelnen Pfade der vorher gefundenen Komponenten
    for(ComponentPort actualComponentPort : foundComponents)
    {
        //Möchte vom entgegengesetzten Port ausgehen
        actualComponentPort.invertPort();
        QList<ComponentPort> newFoundComps;

        //Hinzufügen der gefundenen Objekte vom entgegengesetzten Port
        for(Connection* c : _connections)
        {
            if(actualComponentPort == c->getComponentPortA())
            {
                if(!newFoundComps.contains(c->getComponentPortB()))
                {
                    newFoundComps.append(c->getComponentPortB());

                    for(ComponentPort d : foundComponents)
                    {
                        if(c->getComponentPortB().getOppisiteComponentPort() == d)
                        {
                            _connections.removeOne(c);
                        }
                    }
                }
            }
            else if (actualComponentPort == c->getComponentPortB())
            {
                if(!newFoundComps.contains(c->getComponentPortA()))
                {
                    newFoundComps.append(c->getComponentPortA());

                    for(ComponentPort d : foundComponents)
                    {
                        if(c->getComponentPortA().getOppisiteComponentPort() == d)
                        {
                            _connections.removeOne(c);
                        }
                    }
                }
            }

            lookingForNeighbours(newFoundComps);
           /* qDebug() << _connections.count();

            for(ComponentPort a : newFoundComps)
            {
                qDebug() << a.getComponent()->getName() << "für Component" << d;
            } */

            for(ComponentPort q : newFoundComps)
            {
                if(q.getComponent()->getComponentType() == Component::PowerSupply)
                {
                    newFoundComps.removeOne(q);
                }
            }

            qDebug() << "Gefunden:" << newFoundComps.count();
            qDebug() << "Gehabt:" << foundComponents.count();

            //Wenn soviele Componenten gefunden wurden wie in der Liste waren können wir sagen Schaltung wurde geschlossen
            int i = 0;
            for(ComponentPort cp : foundComponents)
            {
                for(ComponentPort ck : newFoundComps)
                {
                    if(cp.getComponent() == ck.getComponent())
                    {
                        //   qDebug() << ck.getComponent()->getName();
                        i++;
                        break;

                    }
                }
            }
            if(i == foundComponents.count())
            {
                //isEndParallel = true; Ende Parallelschaltung
                break;
            }
        }
        //Berechnung der Werte Addition der Leitwerte
        double zaehler = 0;
        for(int i = 0; i < foundComponents.count(); i++)
        {
            zaehler += 1.0 / foundComponents[i].getComponent()->getValue();
        }
        //Kehrwert der Leitwerte = Widerstand
        zaehler = qPow(zaehler, -1);
        actualImpedanz = zaehler;
        actualComponentPort.invertPort();
    }
}

QList<ComponentPort>Calculator::findConnectedComponents(ComponentPort componentPort, QList<ComponentPort>& connectedComponents)
{
    for(Connection* connection : _connections)
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

//Rekursive Funktion zum finden aller Komponenten, newFound sind die neuen gefundenen ComponentPorts, diese werden am Ende zur foundComponents hinzugefügt
void Calculator::lookingForNeighbours(QList<ComponentPort> &foundComponents)
{
    QList<ComponentPort> newFound;

    for(ComponentPort cp : foundComponents)
    {
        for(Connection* con : _connections)
        {
            if(cp == con->getComponentPortA() && !newFound.contains(con->getComponentPortB()))
            {
                newFound.append(con->getComponentPortB());
                _connections.removeOne(con);
            }
            else if(cp == con->getComponentPortB() && !newFound.contains(con->getComponentPortA()))
            {
                newFound.append(con->getComponentPortA());
                _connections.removeOne(con);
            }
        }
    }

    if(newFound.count() != 0)
    {
        lookingForNeighbours(newFound);

        for(ComponentPort cp : newFound)
        {
            foundComponents.append(cp);
        }
    }
}
