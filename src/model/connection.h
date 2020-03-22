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
#include <model/graphicsobject.h>

#include <algorithm>
#include <QPainter>

class NetworkGraphics;
class Description;
class ComponentPort;

class Connection : public GraphicsObject
{
public:
    Connection(ComponentPort componentPortA, ComponentPort componentPortB, NetworkGraphics* model);

    bool operator==(const Connection& rhs);
    bool operator!=(const Connection& rhs);

    QRectF boundingRect(void) const override;
    void changePortOfComponentPortWithComponent(Component* componentOfComponentPortToChangePortOf);

    //getter
    bool            isThereAComponentOrADescription(int x, int y);
    Component*      getComponentAtPosition(int x, int y);
    Description*    getDescriptionAtPosition(int x, int y);
    QList<QRect>    getHitBoxes(void) const {return _connectionHitbox;}
    ComponentPort   getComponentPortOne(void) const {return _componentPortOne;}
    ComponentPort   getComponentPortTwo(void) const {return _componentPortTwo;}
    bool            hasComponent(Component* searchedComponent);


    static constexpr int _circleRadius = 5;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int pathAnalyse(bool horizontalBeforeVertical);
    void paintSelectionHighlight(QPainter* painter) override;

    void drawHorizontalLines(void);
    void drawLeftOrRight(int leftOrRight);
    void drawVerticalLines(void);
    void drawUpOrDown(int upOrDown);
    void dodgeComponent(void);
    int  horizontalPathAnalysis(int howManyConnections, bool horizontalFirst);
    int  leftOrRightPathAnalysis(int howManyConnections, bool horizontalFirst, int leftOrRight);
    int  verticalPathAnalysis(int howManyConnections, bool horizontalFirst);
    int  upOrDownPathAnalysis(int howManyConnections, bool horizontalFirst, int upOrDown);
    int  pathAnalysisDodgeComponent(int howManyConnections);
    void initializeValues(void);
    void paintStartAndEndPoint(QPainter* painter);

    int signum(int checkedNumber);

    int _diffX = 0;
    int _diffY = 0;

    int _countChangeDirection = 0;

    ComponentPort _componentPortOne;
    ComponentPort _componentPortTwo;

    QPainter* _painter = nullptr;

    QPointF _currentPoint;
    QPointF _startPoint;
    QPointF _endPoint;

    bool _isDodgedBefore = false;
    bool _isDodgedBeforePathAnalyse = false;


    NetworkGraphics* _model = nullptr;
    QList<QRect> _connectionHitbox;

    bool _startHorizontal;
};

#endif // CONNECTION_H
