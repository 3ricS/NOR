#include "connection.h"
#include "model/networkgraphics.h"

Connection::Connection(ComponentPort componentPortA, ComponentPort componentPortB, NetworkGraphics* model) :
        QGraphicsItem(nullptr),
        _componentPortOne(componentPortA), _componentPortTwo(componentPortB), _model(model)
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
    _startPoint = _componentPortOne.getComponent()->getPortPosition(_componentPortOne.getPort());
    _currentPoint = _startPoint;
    _endPoint = _componentPortTwo.getComponent()->getPortPosition(_componentPortTwo.getPort());

    //Punkte an Enden zeichnen
    painter->setBrush(QBrush(Qt::black));
    painter->drawEllipse(_startPoint.toPoint(), _circleRadius, _circleRadius);
    painter->drawEllipse(_endPoint.toPoint(), _circleRadius, _circleRadius);

    _painter = painter;
    /*_startX = start.toPoint().x();
    _startY = start.toPoint().y();
    _endX = end.toPoint().x();
    _endY = end.toPoint().y();
    _currentPosX = start.toPoint().x();
    _currentPosY = start.toPoint().y();*/


    _diffX = _endPoint.x() - _startPoint.x();
    _diffY = _endPoint.y() - _startPoint.y();
    //qDebug() << _diffX << " , " << _diffY;
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
    int xStart = start.toPoint().x();
    int yStart = start.toPoint().y();
    int xEnd = end.toPoint().x();
    int yEnd = end.toPoint().y();

    if(xStart < xEnd)
    {
        xStart = xStart - _circleRadius;
        xEnd = xEnd + _circleRadius;
    } else
    {
        xStart = xStart + _circleRadius;
        xEnd = xEnd - _circleRadius;
    }

    if(yStart < yEnd)
    {
        yStart -= _circleRadius;
        yEnd += _circleRadius;
    } else
    {
        yStart += _circleRadius;
        yEnd -= _circleRadius;
    }

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

bool Connection::isThereAComponentOrADescription(int x, int y)
{
    return getComponentAtPosition(x, y) != nullptr || getDescriptionAtPosition(x, y) != nullptr;
}

Component *Connection::getComponentAtPosition(int x, int y)
{
    for (Component* component : _model->getComponents())
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

DescriptionField *Connection::getDescriptionAtPosition(int x, int y)
{
    for (DescriptionField* description : _model->getDescriptions())
    {
        bool equalX = (description->getXPos() == x);
        bool equalY = (description->getYPos() == y);
        if (equalX && equalY)
        {
            return description;
        }
    }
    return nullptr;
}

void Connection::horizontalRoutine()
{
    if(_diffX > 0)
    {
        while(_diffX != 0)
        {
            if(isStartComponentVertical() || !isThereAComponentOrADescription(_currentPoint.x() + 50, _currentPoint.y()) || _diffX == 50)
            {
                QRect* hitbox = new QRect(_currentPoint.x(), _currentPoint.y() - 5, 50, 10);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() + 50, _currentPoint.y());
                _currentPoint.setX(_currentPoint.x() + 50);
                _diffX -= 50;
                _isDodgedBefore = false;
            }
            else
            {
                if(_diffY == 0)
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
    else if (_diffX < 0)
    {
        while(_diffX != 0)
        {
            if(isStartComponentVertical() || !isThereAComponentOrADescription(_currentPoint.x() - 50, _currentPoint.y()) || _diffX == 50)
            {
                QRect* hitbox = new QRect(_currentPoint.x() - 50, _currentPoint.y() - 5, 50, 10);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() - 50, _currentPoint.y());
                _currentPoint.setX(_currentPoint.x() - 50);
                _diffX += 50;
                _isDodgedBefore = false;
            }
            else
            {
                if(_diffY == 0)
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
    if(_diffY > 0)
    {
        while(_diffY != 0)
        {
            if(!isStartComponentVertical() || !isThereAComponentOrADescription(_currentPoint.x(), _currentPoint.y() + 50) || _diffY == 50)
            {
                QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y(), 10, 50);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() + 50);
                _currentPoint.setY(_currentPoint.y() + 50);
                _diffY -= 50;
                _isDodgedBefore = false;
            }
            else
            {
                if(_diffX == 0)
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
    else if (_diffY < 0)
    {
        while(_diffY != 0)
        {
            if(!isStartComponentVertical() || !isThereAComponentOrADescription(_currentPoint.x(), _currentPoint.y() - 50) || _diffY == 50)
            {
                QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y() - 50, 10, 50);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() - 50);
                _currentPoint.setY(_currentPoint.y() - 50);
                _diffY += 50;
                _isDodgedBefore = false;
            }
            else
            {
                if(_diffX == 0)
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
    if(_diffX == 0)
    {
        if(!_isDodgedBefore)
        {
            QRect* hitbox = new QRect(_currentPoint.x(), _currentPoint.y() - 5, 55, 10);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() + 55, _currentPoint.y());
            _currentPoint.setX(_currentPoint.x() + 55);
            _isDodgedBefore = true;
        }
        if(_diffY < 0)
        {
            QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y() - 100, 10, 100);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() - 100);
            _currentPoint.setY(_currentPoint.y() - 100);
            _diffY += 100;
            if(!isThereAComponentOrADescription(_currentPoint.x() - 55, _currentPoint.y() - 50) || _diffY == 0)
            {
                QRect* hitbox = new QRect(_currentPoint.x()- 55, _currentPoint.y() - 5, 55, 10);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() - 55, _currentPoint.y());
                _currentPoint.setX(_currentPoint.x() - 55);
            }
            else
            {
                dodgeRoutine();
            }
        }
        if(_diffY > 0)
        {
            QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y(), 10, 100);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() + 100);
            _currentPoint.setY(_currentPoint.y() + 100);
            _diffY -= 100;
            if(!isThereAComponentOrADescription(_currentPoint.x() - 55, _currentPoint.y() + 50) || _diffY == 0)
            {
                QRect* hitbox = new QRect(_currentPoint.x() - 55, _currentPoint.y() - 5, 55, 10);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() - 55, _currentPoint.y());
                _currentPoint.setX(_currentPoint.x() - 55);
            }
            else
            {
                dodgeRoutine();
            }
        }
    }
    if(_diffY == 0)
    {
        if(!_isDodgedBefore)
        {
            QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y(), 10, 55);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() + 55);
            _currentPoint.setY(_currentPoint.y() + 55);
            _isDodgedBefore = true;
        }
        if(_diffX < 0)
        {
            QRect* hitbox = new QRect(_currentPoint.x() - 100, _currentPoint.y() - 5, 100, 10);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() - 100, _currentPoint.y());
            _currentPoint.setX(_currentPoint.x() - 100);
            _diffX += 100;
            if(!isThereAComponentOrADescription(_currentPoint.x() - 50, _currentPoint.y() - 55) || _diffX == 0)
            {
                QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y() - 55, 10, 55);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() - 55);
                _currentPoint.setY(_currentPoint.y() - 55);
            }
            else
            {
                dodgeRoutine();
            }
        }
        if(_diffX > 0)
        {
            QRect* hitbox = new QRect(_currentPoint.x(), _currentPoint.y() - 5, 100, 10);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() + 100, _currentPoint.y());
            _currentPoint.setX(_currentPoint.x() + 100);
            _diffX -= 100;
            if(!isThereAComponentOrADescription(_currentPoint.x() + 50, _currentPoint.y() - 55) || _diffX == 0)
            {
                QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y() - 55, 10, 55);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() - 55);
                _currentPoint.setY(_currentPoint.y() - 55);
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
    return _startPoint.toPoint().y() % 100 == 0;
}

bool Connection::operator==(const Connection& rhs)
{
    bool equalInEqualDirection = (_componentPortOne == rhs.getComponentPortOne() &&
                                  _componentPortTwo == rhs.getComponentPortTwo());
    bool equalInOtherDirection = (_componentPortOne == rhs.getComponentPortTwo() &&
                                  _componentPortTwo == rhs.getComponentPortOne());
    return equalInEqualDirection || equalInOtherDirection;
}

bool Connection::operator!=(const Connection& rhs)
{
    return !(operator==(rhs));
}

