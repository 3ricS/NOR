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

    searchingPowerSupply(foundComponents);

    searchingForIndirectNeighbours(foundComponents);

    bool isParallel = foundComponents.count() >= 2;
    if(isParallel)
    {
        parallelAnalysis(foundComponents, actualImpedanz);
        //QMessageBox::about(nullptr, "Berechnung", "Der Gesamtwiderstand des Netzwerkes beträgt : " +
        //                   QString::number(actualImpedanz) + "Ω");
    }

    bool isRow = foundComponents.count() == 1;
    if(isRow)
    {
        rowAnalysis(foundComponents[0], actualImpedanz);
        //QMessageBox::about(nullptr, "Berechnung", "Der Gesamtwiderstand des Netzwerkes beträgt : " +
        //                   QString::number(actualImpedanz) + "Ω");
    }

    bool isNotConnected = foundComponents.count() == 0;
    if(isNotConnected)
    {
        //QMessageBox::about(nullptr, "Fehler", "PowerSupply nicht verbunden.");
        qDebug() << "Calculator.cpp: PowerSupply nicht verbunden";
    }

    _resistanceValue = actualImpedanz;
    // QList<ComponentPort> connectedComponents;
    // connectedComponents = findConnectedComponents(ComponentPort(firstComponent, Component::Port::A),
    // connectedComponents);
}

void Calculator::rowAnalysis(ComponentPort actualComPort, double& actualImpedanz)
{
    actualImpedanz += actualComPort.getComponent()->getValue();

    QList<ComponentPort> foundComponents;
   // QList<ComponentPort> newFoundComponents;

    ComponentPort oppisiteActualPort = actualComPort.getOppisiteComponentPort();

    searchingForDirectRowNeighbours(oppisiteActualPort,foundComponents);
  /*  for (Connection* connection : _connections)
    {
        if(connection->getComponentPortA().getComponent() == actualComPort && connection->getComponentPortB().getComponent() != lastComponent)
        {
            count++;
            nextComponent = connection->getComponentPortB().getComponent();
        }
        else if(connection->getComponentPortTwo().getComponent() == actualComPort && connection->getComponentPortA().getComponent() != lastComponent)
        {
            count++;
            nextComponent = connection->getComponentPortOne().getComponent();
        }
    } */

    if(foundComponents.count() == 1)
    {
        rowAnalysis(foundComponents[0], actualImpedanz);
    }

    if(foundComponents.count() >= 2)
    {
        parallelAnalysis(foundComponents, actualImpedanz);
    }
}

void Calculator::parallelAnalysis(QList<ComponentPort>& foundComponents, double& actualImpedanz)
{
    //ob gefunden wurde
    // bool isEndParallel = false;

    //Analyse der einzelnen Pfade der vorher gefundenen Komponenten
    for(ComponentPort actualOppisteComponentPort : foundComponents)
    {
        //Möchte vom entgegengesetzten Port ausgehen
        ComponentPort actualComponentPort = actualOppisteComponentPort.getOppisiteComponentPort();
        QList<ComponentPort> newFoundComps;

        //Hinzufügen der gefundenen Objekte vom entgegengesetzten Port
        searchingForDirectParallelNeighbours(actualComponentPort, foundComponents, newFoundComps);

        //Entfernen der PowerSupply
        for(ComponentPort q : newFoundComps)
        {
            if(q.getComponent()->getComponentType() == Component::PowerSupply)
            {
                newFoundComps.removeOne(q);
            }
        }
        if(newFoundComps.count() == 1)
        {
            //rowAnalysis(actualComponentPort.getComponent(), actualImpedanz);
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
    actualImpedanz += zaehler;
}


QList<ComponentPort>Calculator::findConnectedComponents(ComponentPort componentPort, QList<ComponentPort>& connectedComponents)
{
    //TODO: dies gehört ins Model: networkgraphics.cpp
    for(Connection* connection : _connections)
    {
        ComponentPort foundComponentPort = ComponentPort(nullptr, Component::Port::null);

        bool found = false;
        if(componentPort.getComponent() == connection->getComponentPortTwo().getComponent())
        {
            Component* foundComponent = connection->getComponentPortOne().getComponent();
            Component::Port foundPort = connection->getComponentPortOne().getPort();
            foundComponentPort = ComponentPort(foundComponent, foundPort);
            found = true;
        }
        else if(componentPort.getComponent() == connection->getComponentPortOne().getComponent())
        {
            Component* foundComponent = connection->getComponentPortTwo().getComponent();
            Component::Port foundPort = connection->getComponentPortTwo().getPort();
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
void Calculator::searchingForIndirectNeighbours(QList<ComponentPort> &foundComponents)
{
    //TODO: dies gehört ins Model: networkgraphics.cpp
    QList<ComponentPort> newFound;

    for(ComponentPort cp : foundComponents)
    {
        for(Connection* con : _connections)
        {
            if(cp == con->getComponentPortOne() && !newFound.contains(con->getComponentPortTwo()))
            {
                newFound.append(con->getComponentPortTwo());
                _connections.removeOne(con);
            }
            else if(cp == con->getComponentPortTwo() && !newFound.contains(con->getComponentPortOne()))
            {
                newFound.append(con->getComponentPortOne());
                _connections.removeOne(con);
            }
        }
    }

    if(newFound.count() != 0)
    {
        searchingForIndirectNeighbours(newFound);

        for(ComponentPort cp : newFound)
        {
            foundComponents.append(cp);
        }
    }
}

void Calculator::searchingForDirectParallelNeighbours(ComponentPort actualComPort, QList<ComponentPort> foundCompPort, QList<ComponentPort> &newFoundCompPort)
{
    for(Connection* c : _connections)
    {
        if(actualComPort == c->getComponentPortOne())
        {
            if(!newFoundCompPort.contains(c->getComponentPortTwo()))
            {
                newFoundCompPort.append(c->getComponentPortTwo());

                for(ComponentPort d : foundCompPort)
                {
                    if(c->getComponentPortTwo().getOppisiteComponentPort() == d)
                    {
                        _connections.removeOne(c);
                    }
                }
            }
        }
        else if (actualComPort == c->getComponentPortTwo())
        {
            if(!newFoundCompPort.contains(c->getComponentPortOne()))
            {
                newFoundCompPort.append(c->getComponentPortOne());

                for(ComponentPort d : foundCompPort)
                {
                    if(c->getComponentPortOne().getOppisiteComponentPort() == d)
                    {
                        _connections.removeOne(c);
                    }
                }
            }
        }

        searchingForIndirectNeighbours(newFoundCompPort);
    }
}

void Calculator::searchingPowerSupply(QList<ComponentPort> &foundComponents)
{
    for (Connection* connection : _connections)
    {
        //Suche nach den ersten Widerständen und entfernen der Verbindungen zum Port A der PowerSupply
        if(((connection->getComponentPortOne().getComponent()->getComponentType() == Component::PowerSupply) &&
                connection->getComponentPortOne().getPort() == Component::A ) ||
           ((connection->getComponentPortTwo().getComponent()->getComponentType() == Component::PowerSupply &&
                   connection->getComponentPortTwo().getPort() == Component::A)))
        {
            if(connection->getComponentPortOne().getComponent()->getComponentType() == Component::PowerSupply)
            {
                foundComponents.append(connection->getComponentPortTwo());
                _connections.removeOne(connection);
            }
            if(connection->getComponentPortTwo().getComponent()->getComponentType() == Component::PowerSupply)
            {
                foundComponents.append(connection->getComponentPortOne());
                _connections.removeOne(connection);
            }

        }
    }
}

void Calculator::searchingForDirectRowNeighbours(ComponentPort actualComPort, QList<ComponentPort>& foundCompPort)
{
    for(Connection* c : _connections)
    {
        if(actualComPort == c->getComponentPortOne())
        {
            foundCompPort.append(c->getComponentPortTwo());
            _connections.removeOne(c);
        }
        else if (actualComPort == c->getComponentPortTwo())
        {
           foundCompPort.append(c->getComponentPortOne());
           _connections.removeOne(c);
        }

        searchingForIndirectNeighbours(foundCompPort);
    }
}

void Calculator::setLists(QList<Connection*> connections, QList<Component*> components)
{
    _connections = connections;
    _components = components;
}
