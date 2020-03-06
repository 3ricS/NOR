#include "defines.h"
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
    _connectionHitbox.clear();
    _startPoint = _componentPortOne.getComponent()->getPortPosition(_componentPortOne.getPort());
    _endPoint = _componentPortTwo.getComponent()->getPortPosition(_componentPortTwo.getPort());

    //Punkte an Enden zeichnen
    painter->setBrush(QBrush(Qt::black));
    painter->drawEllipse(_startPoint.toPoint(), _circleRadius, _circleRadius);
    painter->drawEllipse(_endPoint.toPoint(), _circleRadius, _circleRadius);

    _painter = painter;
    _isDodgedBefore = false;

    initializeValues();

    int horizontalFirst = pathAnalyse(true);

    int countChangesHorizontalFirst = _countChangeDirection;

    initializeValues();

    int verticalFirst = pathAnalyse(false);

    int countChangesVerticalFirst = _countChangeDirection;

    initializeValues();

    if(horizontalFirst < verticalFirst)
    {
        _startHorizontal = true;
        drawHorizontalLines();
    }
    else if(horizontalFirst > verticalFirst)
    {
        _startHorizontal = false;
        drawVerticalLines();
    }
    else if (horizontalFirst == verticalFirst)
    {
        if(countChangesHorizontalFirst <= countChangesVerticalFirst)
        {
            _startHorizontal = true;
            drawHorizontalLines();
        }
        else
        {
            _startHorizontal = false;
            drawVerticalLines();
        }
    }

    if(_isSelected)
    {
        paintHitbox(painter);
    }
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
    return (_componentPortOne.getComponent() == searchedComponent) || (_componentPortTwo.getComponent() == searchedComponent);

}

void Connection::set_isSelected(bool isSelected)
{
     _isSelected = isSelected;
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

int Connection::pathAnalyse(bool horizontalFirst)
{
   int howManyConnections = 0;
   if(horizontalFirst)
   {
       return  horizontalPathAnalysis(howManyConnections, horizontalFirst);
   }
   else
   {
      return verticalPathAnalysis(howManyConnections, horizontalFirst);
   }

}

void Connection::paintHitbox(QPainter *painter)
{
    for(QRect* hitbox : _connectionHitbox)
    {
        QBrush brush;
        brush.setColor(QColor(255, 0, 0, 55));
        brush.setStyle(Qt::BrushStyle::SolidPattern);
        painter->setPen(Qt::NoPen);
        painter->setBrush(brush);
        painter->drawRect(hitbox->x(), hitbox->y(), hitbox->width(), hitbox->height());
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

void Connection::drawHorizontalLines()
{
    if(_diffX > 0)
    {
        drawLeftOrRight(Defines::halfGridLength);
    }
    else if (_diffX < 0)
    {
        drawLeftOrRight(- Defines::halfGridLength);
    }
    if(_startHorizontal)
    {
        drawVerticalLines();
    }
}

void Connection::drawLeftOrRight(int leftOrRight)
{
    while(_diffX != 0)
    {
        if((_diffY != Defines::halfGridLength || _diffY != - Defines::halfGridLength) && _diffX == leftOrRight && ((_diffY > 0 && isThereAComponentOrADescription(_currentPoint.x() + leftOrRight, _currentPoint.y() + Defines::halfGridLength)) || (_diffY < 0 && isThereAComponentOrADescription(_currentPoint.x() + leftOrRight, _currentPoint.y() - Defines::halfGridLength))))
        {
            drawVerticalLines();
        }
        if(!isThereAComponentOrADescription(_currentPoint.x() + leftOrRight, _currentPoint.y()))
        {
            if(leftOrRight > 0)
            {
                QRect* hitbox = new QRect(_currentPoint.x(), _currentPoint.y() - 5, Defines::halfGridLength, 10);
                _connectionHitbox.append(hitbox);
            }
            else
            {
                QRect* hitbox = new QRect(_currentPoint.x() - Defines::halfGridLength, _currentPoint.y() - 5, Defines::halfGridLength, 10);
                _connectionHitbox.append(hitbox);
            }

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() + leftOrRight, _currentPoint.y());
            _currentPoint.setX(_currentPoint.x() + leftOrRight);
            _diffX = _diffX - leftOrRight;
            _isDodgedBefore = false;
        }
        else
        {
            if(_diffY == 0)
            {
                dodgeComponent();
            }
            else
            {
                drawVerticalLines();
            }
        }
    }
}

void Connection::drawVerticalLines()
{
    if(_diffY > 0)
    {
        drawUpOrDown(Defines::halfGridLength);
    }
    else if (_diffY < 0)
    {
        drawUpOrDown(- Defines::halfGridLength);
    }
    if(!_startHorizontal)
    {
        drawHorizontalLines();
    }
}

void Connection::drawUpOrDown(int upOrDown)
{
    while(_diffY != 0)
    {
        if((_diffX != Defines::halfGridLength || _diffX != - Defines::halfGridLength) && _diffY == upOrDown && ((_diffX > 0 && isThereAComponentOrADescription(_currentPoint.x() + Defines::halfGridLength, _currentPoint.y() + upOrDown)) || (_diffX < 0 && isThereAComponentOrADescription(_currentPoint.x() - Defines::halfGridLength, _currentPoint.y() + upOrDown))))
        {
            drawHorizontalLines();
        }
        if(!isThereAComponentOrADescription(_currentPoint.x(), _currentPoint.y() + upOrDown))
        {
            if(upOrDown > 0)
            {
                QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y(), 10, Defines::halfGridLength);
                _connectionHitbox.append(hitbox);
            }
            else
            {
                QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y() - Defines::halfGridLength, 10, Defines::halfGridLength);
                _connectionHitbox.append(hitbox);
            }

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() + upOrDown);
            _currentPoint.setY(_currentPoint.y() + upOrDown);
            _diffY = _diffY - upOrDown;
            _isDodgedBefore = false;
        }
        else
        {
            if(_diffX == 0)
            {
                dodgeComponent();
            }
            else
            {
                drawHorizontalLines();
            }
        }
    }
}

void Connection::dodgeComponent()
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
            QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y() - Defines::gridLength, 10, Defines::gridLength);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() - Defines::gridLength);
            _currentPoint.setY(_currentPoint.y() - Defines::gridLength);
            _diffY += Defines::gridLength;
            if(!isThereAComponentOrADescription(_currentPoint.x() - 55, _currentPoint.y() - Defines::halfGridLength) || _diffY == 0)
            {
                QRect* hitbox = new QRect(_currentPoint.x()- 55, _currentPoint.y() - 5, 55, 10);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() - 55, _currentPoint.y());
                _currentPoint.setX(_currentPoint.x() - 55);
            }
            else
            {
                dodgeComponent();
            }
        }
        if(_diffY > 0)
        {
            QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y(), 10, Defines::gridLength);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() + Defines::gridLength);
            _currentPoint.setY(_currentPoint.y() + Defines::gridLength);
            _diffY -= Defines::gridLength;
            if(!isThereAComponentOrADescription(_currentPoint.x() - 55, _currentPoint.y() + Defines::halfGridLength) || _diffY == 0)
            {
                QRect* hitbox = new QRect(_currentPoint.x() - 55, _currentPoint.y() - 5, 55, 10);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() - 55, _currentPoint.y());
                _currentPoint.setX(_currentPoint.x() - 55);
            }
            else
            {
                dodgeComponent();
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
            QRect* hitbox = new QRect(_currentPoint.x() - Defines::gridLength, _currentPoint.y() - 5, Defines::gridLength, 10);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() - Defines::gridLength, _currentPoint.y());
            _currentPoint.setX(_currentPoint.x() - Defines::gridLength);
            _diffX += Defines::gridLength;
            if(!isThereAComponentOrADescription(_currentPoint.x() - Defines::halfGridLength, _currentPoint.y() - 55) || _diffX == 0)
            {
                QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y() - 55, 10, 55);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() - 55);
                _currentPoint.setY(_currentPoint.y() - 55);
            }
            else
            {
                dodgeComponent();
            }
        }
        if(_diffX > 0)
        {
            QRect* hitbox = new QRect(_currentPoint.x(), _currentPoint.y() - 5, Defines::gridLength, 10);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() + Defines::gridLength, _currentPoint.y());
            _currentPoint.setX(_currentPoint.x() + Defines::gridLength);
            _diffX -= Defines::gridLength;
            if(!isThereAComponentOrADescription(_currentPoint.x() + Defines::halfGridLength, _currentPoint.y() - 55) || _diffX == 0)
            {
                QRect* hitbox = new QRect(_currentPoint.x() - 5, _currentPoint.y() - 55, 10, 55);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() - 55);
                _currentPoint.setY(_currentPoint.y() - 55);
            }
            else
            {
                dodgeComponent();
            }
        }
    }
}

bool Connection::isStartComponentVertical()
{
    return _startPoint.toPoint().y() % 100 == 0;
}

void Connection::initializeValues()
{
    _countChangeDirection = 0;

    _diffX = _endPoint.x() - _startPoint.x();
    _diffY = _endPoint.y() - _startPoint.y();
    _currentPoint = _startPoint;
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

int Connection::horizontalPathAnalysis(int howManyConnections, bool horizontalFirst)
{
    if(_diffX > 0)
    {
        howManyConnections = leftOrRightPathAnalysis(howManyConnections, horizontalFirst, Defines::halfGridLength);
    }
    else if (_diffX < 0)
    {
        howManyConnections = leftOrRightPathAnalysis(howManyConnections, horizontalFirst, - Defines::halfGridLength);
    }
    if(horizontalFirst)
    {
        howManyConnections = verticalPathAnalysis(howManyConnections, horizontalFirst);
        return howManyConnections;
    }
    else
    {
        return howManyConnections;
    }
}

int Connection::leftOrRightPathAnalysis(int howManyConnections, bool horizontalFirst, int leftOrRight)
{
    while(_diffX != 0)
    {
        if(!isThereAComponentOrADescription(_currentPoint.x() + leftOrRight, _currentPoint.y()))
        {
            howManyConnections += 1;
            _currentPoint.setX(_currentPoint.x() + leftOrRight);
            _diffX = _diffX - leftOrRight;
            _isDodgedBeforePathAnalyse = false;
        }
        else
        {
            if(_diffY == 0)
            {
                howManyConnections = pathAnalyseDodgeComponent(howManyConnections);
            }
            else
            {
                _countChangeDirection += 1;
                howManyConnections = verticalPathAnalysis(howManyConnections, horizontalFirst);
            }
        }
    }
    return  howManyConnections;
}

int Connection::verticalPathAnalysis(int howManyConnections, bool horizontalFirst)
{
    if(_diffY > 0)
    {
        howManyConnections = upOrDownPathAnalysis(howManyConnections, horizontalFirst, Defines::halfGridLength);
    }
    else if (_diffY < 0)
    {
        howManyConnections = upOrDownPathAnalysis(howManyConnections, horizontalFirst, - Defines::halfGridLength);
    }
    if(!horizontalFirst)
    {
        howManyConnections = horizontalPathAnalysis(howManyConnections, horizontalFirst);
        return howManyConnections;
    }
    else
    {
        return howManyConnections;
    }
}

int Connection::upOrDownPathAnalysis(int howManyConnections, bool horizontalFirst, int upOrDown)
{
    while(_diffY != 0)
    {
        if(!isThereAComponentOrADescription(_currentPoint.x(), _currentPoint.y() + upOrDown))
        {
            howManyConnections += 1;
            _currentPoint.setY(_currentPoint.y() + upOrDown);
            _diffY = _diffY - upOrDown;
            _isDodgedBeforePathAnalyse = false;
        }
        else
        {
            if(_diffX == 0)
            {
                howManyConnections = pathAnalyseDodgeComponent(howManyConnections);
            }
            else
            {
                _countChangeDirection += 1;
                howManyConnections = horizontalPathAnalysis(howManyConnections, horizontalFirst);
            }
        }
    }
    return howManyConnections;
}

int Connection::pathAnalyseDodgeComponent(int howManyConnections)
{
    if(_diffX == 0)
    {
        if(!_isDodgedBeforePathAnalyse)
        {
            howManyConnections += 1;
            _currentPoint.setX(_currentPoint.x() + 55);
            _isDodgedBeforePathAnalyse = true;
        }
        if(_diffY < 0)
        {
            howManyConnections += 2;
            _currentPoint.setY(_currentPoint.y() - Defines::gridLength);
            _diffY += Defines::gridLength;
            if(!isThereAComponentOrADescription(_currentPoint.x() - 55, _currentPoint.y() - Defines::halfGridLength) || _diffY == 0)
            {
                howManyConnections += 1;
                _currentPoint.setX(_currentPoint.x() - 55);
            }
            else
            {
                howManyConnections = pathAnalyseDodgeComponent(howManyConnections);
            }
        }
        if(_diffY > 0)
        {
            howManyConnections += 2;
            _currentPoint.setY(_currentPoint.y() + Defines::gridLength);
            _diffY -= Defines::gridLength;
            if(!isThereAComponentOrADescription(_currentPoint.x() - 55, _currentPoint.y() + Defines::halfGridLength) || _diffY == 0)
            {
                howManyConnections += 1;
                _currentPoint.setX(_currentPoint.x() - 55);
            }
            else
            {
                howManyConnections = pathAnalyseDodgeComponent(howManyConnections);
            }
        }
    }
    if(_diffY == 0)
    {
        if(!_isDodgedBeforePathAnalyse)
        {
            howManyConnections += 1;
            _currentPoint.setY(_currentPoint.y() + 55);
            _isDodgedBeforePathAnalyse = true;
        }
        if(_diffX < 0)
        {
            howManyConnections += 2;
            _currentPoint.setX(_currentPoint.x() - Defines::gridLength);
            _diffX += Defines::gridLength;
            if(!isThereAComponentOrADescription(_currentPoint.x() - Defines::halfGridLength, _currentPoint.y() - 55) || _diffX == 0)
            {
                howManyConnections += 1;
                _currentPoint.setY(_currentPoint.y() - 55);
            }
            else
            {
                howManyConnections = pathAnalyseDodgeComponent(howManyConnections);
            }
        }
        if(_diffX > 0)
        {
            howManyConnections += 2;
            _currentPoint.setX(_currentPoint.x() + Defines::gridLength);
            _diffX -= Defines::gridLength;
            if(!isThereAComponentOrADescription(_currentPoint.x() + Defines::halfGridLength, _currentPoint.y() - 55) || _diffX == 0)
            {
                howManyConnections += 1;
                _currentPoint.setY(_currentPoint.y() - 55);
            }
            else
            {
                howManyConnections = pathAnalyseDodgeComponent(howManyConnections);
            }
        }
    }
    return howManyConnections;
}

