/*!
 * \copyright   PHWT
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief       Mithilfe Objekte dieser Klasse werden zwei Komponenten der Schaltung miteinander verbunden
 *
 * Die Klasse Connection besitzt zwei ComponentPorts, welche miteinander verbunden werden. Jeder ComponentPort
 * zeigt auf einen Komponenten und den jeweiligen Port in der Schaltung. Damit wird festgelegt, zwischen welchen beiden
 * Komponenten bei welchem Port die Verbindung hergestellt wird.
 * Will man wissen, wer mit wem verbunden ist, kann man sich von der Connection den ComponentPort am Anfang und Ende geben Lassen
 * (getComponentPortOne(), getComponentPortTwo()). Von diesen kann man nun den Component und den Port auslesen. Damit
 * weiß man genau, welche Komponent an welchem Port womit verbunden ist.
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <model/ComponentPort.h>

#include <QPainter>
#include <algorithm>

class Connection : public QGraphicsItem
{
public:
    Connection(ComponentPort componentPortA, ComponentPort componentPortB);

    //TODO: boundingRect muss noch beschrieben werden
    QRectF boundingRect(void) const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void changePortOfComponentPortWithComponent(Component* componentOfComponentPortToChangePortOf);

    //Methoden
    ComponentPort getComponentPortOne(void) const {return _componentPortOne;}
    ComponentPort getComponentPortTwo(void) const {return _componentPortTwo;}
    bool hasComponent(Component* searchedComponent);

private:
    ComponentPort _componentPortOne;
    ComponentPort _componentPortTwo;
};

#endif // CONNECTION_H
