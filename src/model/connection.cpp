#include "connection.h"

Connection::Connection(ComponentPort componentPortA, ComponentPort componentPortB) :
        QGraphicsItem(nullptr),
        _componentPortOne(componentPortA), _componentPortTwo(componentPortB)
{
}

/*!
 * \brief   Zeichnet die Verbindung in QGraphicsScene
 * \param[inout]    painter Objekt, das die einzelnen Elemente zeichnet
 * \param[]         option
 * \param[]         widget
 */
void Connection::paint(QPainter* painter, [[maybe_unused]] const QStyleOptionGraphicsItem* option, [[maybe_unused]] QWidget* widget)
{
    //TODO: Funktion mit mehreren Rückgabewerten für xStart, xEnd, ...
    QPointF start = _componentPortOne.getComponent()->getPortPosition(_componentPortOne.getPort());
    QPointF end = _componentPortTwo.getComponent()->getPortPosition(_componentPortTwo.getPort());
    int xStart = static_cast<int>(start.x());
    int yStart = static_cast<int>(start.y());
    int xEnd = static_cast<int>(end.x());
    int yEnd = static_cast<int>(end.y());

    //first calculate the coordinates of the middle
    int xMiddle = static_cast<int>(xStart + 0.5 * (xEnd - xStart));
    int yMiddle = static_cast<int>(yStart + 0.5 * (yEnd - yStart));

    //draw the four lines to make the connection square
    painter->drawLine(xStart, yStart, xStart, yMiddle);
    painter->drawLine(xStart, yMiddle, xMiddle, yMiddle);
    painter->drawLine(xMiddle, yMiddle, xEnd, yMiddle);
    painter->drawLine(xEnd, yMiddle, xEnd, yEnd);
}

/*!
 * \brief   Gibt an, ob die Connection den übergebenen Komponenten der Schaltung enthält
 *
 * \param[in]   searchedComponent ist eine Komponente, die in der Verbindung gesucht wird.
 * \return      Gibt einen boolschen Wert zurück, ob der übergebene Component in der Verbindung enthalten ist
 *
 * Ist die übergebene Komponente der Schatlung einer der Verbindungspartner dieser Verbindung, gibt die Methode true zurück, sonst false.
 */
bool Connection::hasComponent(Component* searchedComponent)
{
    return _componentPortOne.getComponent() == searchedComponent || (_componentPortTwo.getComponent() == searchedComponent);
}

/*!
 * \brief   Gibt ein Rechteck an, in dem sich die Verbindung in der QGraphicsScene befindet
 *
 * \return  Gibt Rechteck zurück, in dem sich die Verbindung in der QGraphicsScene befindet
 */
QRectF Connection::boundingRect(void) const
{
    QPointF start = _componentPortOne.getComponent()->getPortPosition(_componentPortOne.getPort());
    QPointF end = _componentPortTwo.getComponent()->getPortPosition(_componentPortTwo.getPort());
    int xStart = static_cast<int>(start.x());
    int yStart = static_cast<int>(start.x());
    int xEnd = static_cast<int>(end.x());
    int yEnd = static_cast<int>(end.y());

    return QRectF(std::min(xStart, xEnd), std::min(yStart, yEnd), abs(xEnd - xStart), abs(yEnd - yStart));
}

/*!
 * \brief   Wechselt den Port, mit welchem die Verbindung verbunden ist, bei dem übergebenen Component.
 *
 * \param[inout]   componentOfComponentPortToChangePortOf Komponente der Schaltung, an dem der Port der Verbindung getauscht werden soll
 *
 * Die Verbindung zeigt jeweils auf eine Component und einen Port des Components, mit dem es verbunden ist.
 * Soll der Port bei einem Component getauscht werden, wird diese Methode aufgerufen. Ist der übergebene Component
 * einer der Components, die in dieser Verbindung enthalten sind, wird der Port dieses Components getauscht.
 */
void Connection::changePortOfComponentPortWithComponent(Component* componentOfComponentPortToChangePortOf)
{
    if(componentOfComponentPortToChangePortOf == _componentPortOne.getComponent())
    {
        _componentPortOne.invertPort();
    } else if(componentOfComponentPortToChangePortOf == _componentPortTwo.getComponent())
    {
        _componentPortTwo.invertPort();
    }
}
