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
    for(Connection* connection : _connections)
    {
        //Suche nach dem ersten Widerstand
        if((connection->getComponentA()->getComponentType() || connection->getComponentB()->getComponentType()) == Component::PowerSupply)
        {
            if(connection->getComponentA()->getComponentType() == Component::PowerSupply)
            {
               firstComponent = connection->getComponentB();
            }
            else
            {
                firstComponent = connection->getComponentA();
            }
            _connections.removeOne(connection);
            break;
        }
    }
    rowAnalysis(firstComponent, actualImpedanz);
    _resistanceValue = actualImpedanz;
}

void Calculator::rowAnalysis(Component* comp, double& actualImpedanz)
{
    int count = 0;
    Component* nextComponent = nullptr;
    for(Connection* connection : _connections)
    {
        if(connection->getComponentA() == comp)
        {
            count++;
            nextComponent = connection->getComponentB();
        }
        else if(connection->getComponentB() == comp)
        {
            nextComponent = connection->getComponentA();
        }
    }
    if(count == 1)
    {
        actualImpedanz += comp->getValue();
        rowAnalysis(nextComponent, actualImpedanz);

    }
    qDebug() << actualImpedanz;
}
