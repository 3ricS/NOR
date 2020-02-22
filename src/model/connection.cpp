#include "connection.h"

Connection::Connection(ComponentPort componentPortA, ComponentPort componentPortB, QList<Component*> componentList) :
        QGraphicsItem(nullptr),
        _componentPortOne(componentPortA), _componentPortTwo(componentPortB), _componentList(componentList)
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
    _painter = painter;
    _startX = start.toPoint().x();
    _startY = start.toPoint().y();
    _endX = end.toPoint().x();
    _endY = end.toPoint().y();
    _currentPosX = start.toPoint().x();
    _currentPosY = start.toPoint().y();

    _difX = _endX - _startX;;
    _difY = _endY - _startY;
    qDebug() << _difX << " , " << _difY;
    horizontalRoutine();
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

Component* Connection::getComponentAtPosition(int x, int y)
{
    for (Component* component : _componentList)
    {
        bool equalX = (component->getXPosition() == x);
        bool equalY = (component->getYPosition() == y);
        if (equalX && equalY)
        {
            return component;
        }
    }
    return nullptr;
}

bool Connection::isThereAComponent(int x, int y)
{
    return getComponentAtPosition(x, y) != nullptr;
}

void Connection::horizontalRoutine()
{
    if(_difX > 0)
    {
        while(_difX != 0)
        {
            if(isStartComponentVertical() || !isThereAComponent(_currentPosX + 50, _currentPosY) || _difX == 100)
            {
                _painter->drawLine(_currentPosX, _currentPosY, _currentPosX + 50, _currentPosY);
                _currentPosX += 50;
                _difX -= 50;
                _isDodgedBefore = false;
            }
            else
            {
                if(_difY == 0)
                {
                    dodgeRoutine();
                }
                else
                {
                    verticalRoutine();
                }
            }
        }
    }
    else if (_difX < 0)
    {
        while(_difX != 0)
        {
            if(isStartComponentVertical() || !isThereAComponent(_currentPosX - 50, _currentPosY) || _difX == 100)
            {
                _painter->drawLine(_currentPosX, _currentPosY, _currentPosX - 50, _currentPosY);
                _currentPosX -= 50;
                _difX += 50;
                _isDodgedBefore = false;
            }
            else
            {
                if(_difY == 0)
                {
                    dodgeRoutine();
                }
                else
                {
                    verticalRoutine();
                }
            }
        }
    }
    verticalRoutine();
}

void Connection::verticalRoutine()
{
    if(_difY > 0)
    {
        while(_difY != 0)
        {
            if(!isStartComponentVertical() || !isThereAComponent(_currentPosX, _currentPosY + 50) || _difY == 100)
            {
                _painter->drawLine(_currentPosX, _currentPosY, _currentPosX, _currentPosY + 50);
                _currentPosY += 50;
                _difY -= 50;
                _isDodgedBefore = false;
            }
            else
            {
                if(_difX == 0)
                {
                    dodgeRoutine();
                }
                else
                {
                    horizontalRoutine();
                }
            }
        }
    }
    else if (_difY < 0)
    {
        while(_difY != 0)
        {
            if(!isStartComponentVertical() || !isThereAComponent(_currentPosX, _currentPosY - 50) || _difY == 100)
            {
                _painter->drawLine(_currentPosX, _currentPosY, _currentPosX, _currentPosY - 50);
                _currentPosY -= 50;
                _difY += 50;
                _isDodgedBefore = false;
            }
            else
            {
                if(_difX == 0)
                {
                    dodgeRoutine();
                }
                else
                {
                    horizontalRoutine();
                }
            }
        }
    }
}

void Connection::dodgeRoutine()
{
    if(_difX == 0)
    {
        if(!_isDodgedBefore)
        {
            _painter->drawLine(_currentPosX, _currentPosY, _currentPosX + 55, _currentPosY);
            _currentPosX += 55;
            _isDodgedBefore = true;
        }
        if(_difY < 0)
        {
            _painter->drawLine(_currentPosX, _currentPosY, _currentPosX, _currentPosY - 100);
            _currentPosY -= 100;
            _difY += 100;
            if(!isThereAComponent(_currentPosX - 55, _currentPosY - 50))
            {
                _painter->drawLine(_currentPosX, _currentPosY, _currentPosX - 55, _currentPosY);
                _currentPosX -= 55;
            }
            else
            {
                dodgeRoutine();
            }
        }
        if(_difY > 0)
        {
            _painter->drawLine(_currentPosX, _currentPosY, _currentPosX, _currentPosY + 100);
            _currentPosY += 100;
            _difY -= 100;
            if(!isThereAComponent(_currentPosX - 55, _currentPosY + 50))
            {
                _painter->drawLine(_currentPosX, _currentPosY, _currentPosX - 55, _currentPosY);
                _currentPosX -= 55;
            }
            else
            {
                dodgeRoutine();
            }
        }
    }
    if(_difY == 0)
    {
        if(!_isDodgedBefore)
        {
            _painter->drawLine(_currentPosX, _currentPosY, _currentPosX, _currentPosY + 55);
            _currentPosY += 55;
            _isDodgedBefore = true;
        }
        if(_difX < 0)
        {
            _painter->drawLine(_currentPosX, _currentPosY, _currentPosX - 100, _currentPosY);
            _currentPosX -= 100;
            _difX += 100;
            if(!isThereAComponent(_currentPosX - 50, _currentPosY - 55))
            {
                _painter->drawLine(_currentPosX, _currentPosY, _currentPosX, _currentPosY - 55);
                _currentPosY -= 55;
            }
            else
            {
                dodgeRoutine();
            }
        }
        if(_difX > 0)
        {
            _painter->drawLine(_currentPosX, _currentPosY, _currentPosX + 100, _currentPosY);
            _currentPosX += 100;
            _difX -= 100;
            if(!isThereAComponent(_currentPosX + 50, _currentPosY - 55))
            {
                _painter->drawLine(_currentPosX, _currentPosY, _currentPosX, _currentPosY - 55);
                _currentPosY -= 55;
            }
            else
            {
                dodgeRoutine();
            }
        }
    }
}

bool Connection::isStartComponentVertical()
{
    if(_startY % 100 == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

