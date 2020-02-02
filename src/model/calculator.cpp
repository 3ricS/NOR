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
    for(Connection* c : _connections)
    {
        //Suche nach dem ersten Widerstand
        if((c->getComponentA()->getComponentType() || c->getComponentB()->getComponentType()) == Component::PowerSupply)
        {
            if(c->getComponentA()->getComponentType() == Component::PowerSupply)
            {
               firstComponent = c->getComponentB();
            }
            else
            {
                firstComponent = c->getComponentA();
            }
            _connections.removeOne(c);
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
    for(Connection* c : _connections)
    {
        if(c->getComponentA() == comp)
        {
            count++;
            nextComponent = c->getComponentB();
        }
        else if(c->getComponentB() == comp)
        {
            nextComponent = c->getComponentA();
        }
    }
    if(count == 1)
    {
        actualImpedanz += comp->getValue();
        rowAnalysis(nextComponent, actualImpedanz);

    }
    qDebug() << actualImpedanz;
}
