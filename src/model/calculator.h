#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <model/component.h>
#include <model/connection.h>
#include <model/ComponentPort.h>

class Calculator
{
public:
    Calculator(QList<Connection*> connections, QList<Component*> components);

    //Getter
    double getResistanceValue(void) const {return _resistanceValue;}

    //Methoden
    void calculate(void);

private:
    //Variables
    QList<Component*> _components;
    QList<Connection*> _connections;
    double _resistanceValue;

    //Methoden
    void rowAnalysis(Component* comp, double& actualImpedanz, Component* lastComponent = nullptr);
    void parallelAnalysis(QList<ComponentPort> &foundComponents, double& actualImpedanz);
    QList<ComponentPort> findConnectedComponents(ComponentPort componentPort, QList<ComponentPort>& connectedComponents);
    void lookingForNeighbours(QList<ComponentPort> &foundComponents);

};

#endif // CALCULATOR_H
