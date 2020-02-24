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

#include <model/componentport.h>

#include <QPainter>
#include <algorithm>

class NetworkGraphics;
class DescriptionField;
class ComponentPort;

class Connection : public QGraphicsItem
{
public:
    Connection(ComponentPort componentPortA, ComponentPort componentPortB, NetworkGraphics* model);

    bool operator==(const Connection& rhs);
    bool operator!=(const Connection& rhs);

    QRectF boundingRect(void) const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void changePortOfComponentPortWithComponent(Component* componentOfComponentPortToChangePortOf);

    //Methoden
    bool isThereAComponentOrADescription(int x, int y);
    Component* getComponentAtPosition(int x, int y);
    DescriptionField* getDescriptionAtPosition(int x, int y);
    QList<QRect*> getHitboxList(void) const {return _connectionHitbox;}
    ComponentPort getComponentPortOne(void) const {return _componentPortOne;}
    ComponentPort getComponentPortTwo(void) const {return _componentPortTwo;}
    bool hasComponent(Component* searchedComponent);
    static constexpr int _circleRadius = 5;

private:
    void horizontalRoutine(void);
    void verticalRoutine(void);
    void dodgeRoutine(void);
    bool isStartComponentVertical(void);

    int _diffX = 0;
    int _diffY = 0;

    ComponentPort _componentPortOne;
    ComponentPort _componentPortTwo;

    QPainter* _painter = nullptr;

    QPointF _currentPoint;
    QPointF _startPoint;
    QPointF _endPoint;

    bool _isDodgedBefore = false;

    NetworkGraphics* _model = nullptr;
    QList<QRect*> _connectionHitbox;
};

#endif // CONNECTION_H
