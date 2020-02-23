/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Der Calculator berechnet den Gesamtwiderstand des erstellten Netzwerkes
 *
 */
#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QtMath>
#include <QMessageBox>

#include <model/component.h>
#include <model/connection.h>
#include <model/ComponentPort.h>

class Calculator
{
public:
    Calculator(QList<Connection*> connections, QList<Component*> components);

    //Getter
    double getResistanceValue(void) const {return _resistanceValue;}

    void setLists(QList<Connection*> connections, QList<Component*> components);

    //Methoden
    void calculate(void);

private:
    //Variables
    QList<Component*> _components;
    QList<Connection*> _connections;
    double _resistanceValue;

    //Methoden
    void rowAnalysis(ComponentPort actualComPort, double& actualImpedanz);
    void parallelAnalysis(QList<ComponentPort> &foundComponents, double& actualImpedanz);
    QList<ComponentPort> findConnectedComponents(ComponentPort componentPort, QList<ComponentPort>& connectedComponents);

    //Suche nach Nachbarn, die nicht direkt mit dem aktuellen ComponentPort verbunden sind
    void searchingForIndirectNeighbours(QList<ComponentPort> &foundComponents);

    //Suche nach Nachbarn, die direkt mit dem aktuellen ComponentPort verbunden sind
    void searchingForDirectParallelNeighbours(ComponentPort actualComPort, QList<ComponentPort> foundCompPort, QList<ComponentPort> &newFoundCompPort);
    void searchingForDirectRowNeighbours(ComponentPort actualComPort, QList<ComponentPort> &foundCompPort);

    void searchingPowerSupply(QList<ComponentPort>& foundComponents);

};

#endif // CALCULATOR_H
