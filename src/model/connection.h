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
    Connection(ComponentPort componentPortA, ComponentPort componentPortB, QList<Component*> componentList);

    //TODO: boundingRect muss noch beschrieben werden
    QRectF boundingRect(void) const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void changePortOfComponentPortWithComponent(Component* componentOfComponentPortToChangePortOf);

    //Methoden
    QList<QRect*> getHitboxList(void) const {return _connectionHitbox;}
    ComponentPort getComponentPortOne(void) const {return _componentPortOne;}
    ComponentPort getComponentPortTwo(void) const {return _componentPortTwo;}
    bool hasComponent(Component* searchedComponent);
    static constexpr int _circleRadius = 3;

private:

    Component* getComponentAtPosition(int x, int y);
    bool isThereAComponent(int x, int y);

    void horizontalRoutine(void);
    void verticalRoutine(void);
    void dodgeRoutine(void);
    bool isStartComponentVertical(void);

    int _diffX = 0;
    int _diffY = 0;

    ComponentPort _componentPortOne;
    ComponentPort _componentPortTwo;

    QPainter* _painter = nullptr;

    int _startX = 0;
    int _startY = 0;
    int _endX = 0;
    int _endY = 0;

    bool _isDodgedBefore = false;

    int _currentPosX = 0;
    int _currentPosY = 0;

    QList<Component*> _componentList;
    QList<QRect*> _connectionHitbox;
};

#endif // CONNECTION_H
