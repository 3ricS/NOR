#include "defines.h"
#include "connection.h"
#include "model/networkgraphics.h"

Connection::Connection(ComponentPort componentPortA, ComponentPort componentPortB, NetworkGraphics* model) :
        GraphicsObject(nullptr),
        _componentPortOne(componentPortA), _componentPortTwo(componentPortB), _model(model)
{
}

void Connection::paint(QPainter* painter, [[maybe_unused]] const QStyleOptionGraphicsItem* option,
                       [[maybe_unused]] QWidget* widget)
{
    _connectionHitbox.clear();
    _startPoint = _componentPortOne.getComponent()->getPortPosition(_componentPortOne.getPort());
    _endPoint = _componentPortTwo.getComponent()->getPortPosition(_componentPortTwo.getPort());

    _painter = painter;
    _isDodgedBefore = false;

    initializeValues();

    int horizontalFirst = pathAnalyse(true);

    int countChangesHorizontalFirst = _countChangeDirection;

    initializeValues();

    int verticalFirst = pathAnalyse(false);

    int countChangesVerticalFirst = _countChangeDirection;

    initializeValues();

    if (horizontalFirst < verticalFirst)
    {
        _startHorizontal = true;
        drawHorizontalLines();
    }
    else if (horizontalFirst > verticalFirst)
    {
        _startHorizontal = false;
        drawVerticalLines();
    }
    else if (horizontalFirst == verticalFirst)
    {
        if (countChangesHorizontalFirst <= countChangesVerticalFirst)
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

    if (_isSelected)
    {
        paintSelectionHighlight(painter);
    }


    paintStartAndEndPoint(painter);
}

/*!
 * \brief   Gibt an, ob die Connection den übergebenen Komponenten der Schaltung enthält
 *
 * \param   searchedComponent ist eine Komponente, die in der Verbindung gesucht wird.
 * \return  Gibt einen boolschen Wert zurück, ob der übergebene Component in der Verbindung enthalten ist
 *
 * Ist die übergebene Komponente der Schatlung einer der Verbindungspartner dieser Verbindung, gibt die Methode true zurück, sonst false.
 */
bool Connection::hasComponent(Component* searchedComponent)
{
    return (_componentPortOne.getComponent() == searchedComponent) ||
           (_componentPortTwo.getComponent() == searchedComponent);

}

/*!
 * \brief   Gibt ein Rechteck an, in dem sich die Verbindung in der QGraphicsScene befindet.
 *
 * \return  Gibt ein Rechteck zurück, in dem sich die Verbindung in der QGraphicsScene befindet
 */
QRectF Connection::boundingRect(void) const
{
    QPointF start = _componentPortOne.getComponent()->getPortPosition(_componentPortOne.getPort());
    QPointF end = _componentPortTwo.getComponent()->getPortPosition(_componentPortTwo.getPort());
    int xStart = start.toPoint().x();
    int yStart = start.toPoint().y();
    int xEnd = end.toPoint().x();
    int yEnd = end.toPoint().y();

    if (xStart < xEnd)
    {
        xStart = xStart - _circleRadius;
        xEnd = xEnd + _circleRadius;
    }
    else
    {
        xStart = xStart + _circleRadius;
        xEnd = xEnd - _circleRadius;
    }

    if (yStart < yEnd)
    {
        yStart -= _circleRadius;
        yEnd += _circleRadius;
    }
    else
    {
        yStart += _circleRadius;
        yEnd -= _circleRadius;
    }

    return QRectF(std::min(xStart, xEnd), std::min(yStart, yEnd), abs(xEnd - xStart), abs(yEnd - yStart));
}

/*!
 * \brief   Wechselt den Port des übergebenen Component, mit welchem die Verbindung verbunden ist.
 *
 * \param   componentOfComponentPortToChangePortOf Komponente der Schaltung, an dem der Port der Verbindung getauscht werden soll
 *
 * Die Verbindung zeigt jeweils auf eine Component und einen Port des Components, mit dem es verbunden ist.
 * Soll der Port bei einem Component getauscht werden, wird diese Methode aufgerufen. Ist der übergebene Component
 * einer der Components, die in dieser Verbindung enthalten sind, wird der Port dieses Components getauscht.
 */
void Connection::changePortOfComponentPortWithComponent(Component* componentOfComponentPortToChangePortOf)
{
    if (componentOfComponentPortToChangePortOf == _componentPortOne.getComponent())
    {
        _componentPortOne.invertPort();
    }
    else if (componentOfComponentPortToChangePortOf == _componentPortTwo.getComponent())
    {
        _componentPortTwo.invertPort();
    }
}


/*!
 * \brief Prüft ob sich an einer bestimmten Koordinate, eine Komponente oder Textfeld befindet.
 *
 * \param   x   ist die Soll-X-Koordinate
 * \param   y   ist die Soll-Y-Koordinate
 * \return Gibt zurück, ob sich an der Gitterposition eine Komponente oder Textfeld befindet.
 *
 */
bool Connection::isThereAComponentOrADescription(int x, int y)
{
    return getComponentAtPosition(x, y) != nullptr || getDescriptionAtPosition(x, y) != nullptr;
}

/*!
 * \brief Liefert ein Component, welches an der Soll-Position ist.
 *
 * \param   x   ist die Soll-X-Koordinate
 * \param   y   ist die Soll-Y-Koordinate
 * \return  Gibt das Component an der Soll-Position zurück.
 */
Component* Connection::getComponentAtPosition(int x, int y)
{
    for (Component* component : _model->getComponents())
    {
        bool equalX = (component->getPosition().x() == x);
        bool equalY = (component->getPosition().y() == y);
        if (equalX && equalY)
        {
            return component;
        }
    }
    return nullptr;
}

/*!
 * \brief Liefert eine Description, welches an der Soll-Position ist.
 *
 * \param   x   ist die Soll-X-Koordinate
 * \param   y   ist die Soll-Y-Koordinate
 * \return  Gibt die Description an der Soll-Position zurück.
 */
Description* Connection::getDescriptionAtPosition(int x, int y)
{
    for (Description* description : _model->getDescriptions())
    {
        bool equalX = (description->getPosition().x() == x);
        bool equalY = (description->getPosition().y() == y);
        if (equalX && equalY)
        {
            return description;
        }
    }
    return nullptr;
}

int Connection::pathAnalyse(bool horizontalBeforeVertical)
{
    int howManyConnections = 0;
    if (horizontalBeforeVertical)
    {
        return horizontalPathAnalysis(howManyConnections, horizontalBeforeVertical);
    }
    else
    {
        return verticalPathAnalysis(howManyConnections, horizontalBeforeVertical);
    }

}

void Connection::paintSelectionHighlight(QPainter* painter)
{
    for (QRect hitbox : _connectionHitbox)
    {
        QBrush brush;
        brush.setColor(QColor(255, 0, 0, 55));
        brush.setStyle(Qt::BrushStyle::SolidPattern);
        painter->setPen(Qt::NoPen);
        painter->setBrush(brush);
        painter->drawRect(hitbox.x(), hitbox.y(), hitbox.width(), hitbox.height());
    }
}

void Connection::drawHorizontalLines(void)
{
    int prefix = signum(_diffX);
    if (_diffX != 0)
    {
        drawLeftOrRight(prefix * (Defines::gridLength / 2));
    }
    if (_startHorizontal)
    {
        drawVerticalLines();
    }
}

void Connection::drawLeftOrRight(int leftOrRight)
{
    while (_diffX != 0)
    {
        if ((_diffY != (Defines::gridLength / 2) || _diffY != -(Defines::gridLength / 2)) && _diffX == leftOrRight &&
            ((_diffY > 0 && isThereAComponentOrADescription(_currentPoint.x() + leftOrRight,
                                                            _currentPoint.y() + (Defines::gridLength / 2))) ||
             (_diffY < 0 && isThereAComponentOrADescription(_currentPoint.x() + leftOrRight,
                                                            _currentPoint.y() - (Defines::gridLength / 2)))))
        {
            drawVerticalLines();
        }
        if (!isThereAComponentOrADescription(_currentPoint.x() + leftOrRight, _currentPoint.y()))
        {
            if (leftOrRight > 0)
            {
                QRect hitbox (_currentPoint.x(), _currentPoint.y() - 5, (Defines::gridLength / 2), 10);
                _connectionHitbox.append(hitbox);
            }
            else
            {
                QRect hitbox(_currentPoint.x() - (Defines::gridLength / 2), _currentPoint.y() - 5,
                                          (Defines::gridLength / 2), 10);
                _connectionHitbox.append(hitbox);
            }

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() + leftOrRight,
                               _currentPoint.y());
            _currentPoint.setX(_currentPoint.x() + leftOrRight);
            _diffX = _diffX - leftOrRight;
            _isDodgedBefore = false;
        }
        else
        {
            if (_diffY == 0)
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

void Connection::drawVerticalLines(void)
{
    int prefix = signum(_diffY);
    if (_diffY != 0)
    {
        drawUpOrDown(prefix * (Defines::gridLength / 2));
    }
    if (!_startHorizontal)
    {
        drawHorizontalLines();
    }
}

void Connection::drawUpOrDown(int upOrDown)
{
    while (_diffY != 0)
    {
        if ((_diffX != (Defines::gridLength / 2) || _diffX != -(Defines::gridLength / 2)) && _diffY == upOrDown &&
            ((_diffX > 0 && isThereAComponentOrADescription(_currentPoint.x() + (Defines::gridLength / 2),
                                                            _currentPoint.y() + upOrDown)) || (_diffX < 0 && isThereAComponentOrADescription(_currentPoint.x() - (Defines::gridLength / 2),
                                                            _currentPoint.y() + upOrDown))))
        {
            drawHorizontalLines();
        }
        if (!isThereAComponentOrADescription(_currentPoint.x(), _currentPoint.y() + upOrDown))
        {
            if (upOrDown > 0)
            {
                QRect hitbox(_currentPoint.x() - 5, _currentPoint.y(), 10, (Defines::gridLength / 2));
                _connectionHitbox.append(hitbox);
            }
            else
            {
                QRect hitbox(_currentPoint.x() - 5, _currentPoint.y() - (Defines::gridLength / 2), 10,
                                          (Defines::gridLength / 2));
                _connectionHitbox.append(hitbox);
            }

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() + upOrDown);
            _currentPoint.setY(_currentPoint.y() + upOrDown);
            _diffY = _diffY - upOrDown;
            _isDodgedBefore = false;
        }
        else
        {
            if (_diffX == 0)
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

void Connection::dodgeComponent(void)
{
    if (_diffX == 0)
    {
        if (!_isDodgedBefore)
        {
            QRect hitbox(_currentPoint.x(), _currentPoint.y() - 5, Defines::gridLength / 2, 10);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() + (Defines::gridLength / 2), _currentPoint.y());
            _currentPoint.setX(_currentPoint.x() + (Defines::gridLength / 2));
            _isDodgedBefore = true;
        }
        if (_diffY < 0)
        {
            QRect hitbox(_currentPoint.x() - 5, _currentPoint.y() - Defines::gridLength, 10, Defines::gridLength);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(),
                               _currentPoint.y() - Defines::gridLength);
            _currentPoint.setY(_currentPoint.y() - Defines::gridLength);
            _diffY += Defines::gridLength;
            if (!isThereAComponentOrADescription(_currentPoint.x() - (Defines::gridLength / 2),
                                                 _currentPoint.y() - (Defines::gridLength / 2)) || _diffY == 0)
            {
                QRect hitbox(_currentPoint.x() - (Defines::gridLength / 2), _currentPoint.y() - 5, (Defines::gridLength / 2), 10);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() - (Defines::gridLength / 2), _currentPoint.y());
                _currentPoint.setX(_currentPoint.x() - (Defines::gridLength / 2));
            }
            else
            {
                dodgeComponent();
            }
        }
        if (_diffY > 0)
        {
            QRect hitbox(_currentPoint.x() - 5, _currentPoint.y(), 10, Defines::gridLength);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(),
                               _currentPoint.y() + Defines::gridLength);
            _currentPoint.setY(_currentPoint.y() + Defines::gridLength);
            _diffY -= Defines::gridLength;
            if (!isThereAComponentOrADescription(_currentPoint.x() - (Defines::gridLength / 2),
                                                 _currentPoint.y() + (Defines::gridLength / 2)) || _diffY == 0)
            {
                QRect hitbox(_currentPoint.x() - (Defines::gridLength / 2), _currentPoint.y() - 5, (Defines::gridLength / 2), 10);
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() - (Defines::gridLength / 2), _currentPoint.y());
                _currentPoint.setX(_currentPoint.x() - (Defines::gridLength / 2));
            }
            else
            {
                dodgeComponent();
            }
        }
    }
    if (_diffY == 0)
    {
        if (!_isDodgedBefore)
        {
            QRect hitbox(_currentPoint.x() - 5, _currentPoint.y(), 10, (Defines::gridLength / 2));
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() + (Defines::gridLength / 2));
            _currentPoint.setY(_currentPoint.y() + (Defines::gridLength / 2));
            _isDodgedBefore = true;
        }
        if (_diffX < 0)
        {
            QRect hitbox(_currentPoint.x() - Defines::gridLength, _currentPoint.y() - 5,
                                      Defines::gridLength, 10);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() - Defines::gridLength,
                               _currentPoint.y());
            _currentPoint.setX(_currentPoint.x() - Defines::gridLength);
            _diffX += Defines::gridLength;
            if (!isThereAComponentOrADescription(_currentPoint.x() - (Defines::gridLength / 2),
                                                 _currentPoint.y() - (Defines::gridLength / 2)) || _diffX == 0)
            {
                QRect hitbox(_currentPoint.x() - 5, _currentPoint.y() - (Defines::gridLength / 2), 10, (Defines::gridLength / 2));
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() - (Defines::gridLength / 2));
                _currentPoint.setY(_currentPoint.y() - (Defines::gridLength / 2));
            }
            else
            {
                dodgeComponent();
            }
        }
        if (_diffX > 0)
        {
            QRect hitbox(_currentPoint.x(), _currentPoint.y() - 5, Defines::gridLength, 10);
            _connectionHitbox.append(hitbox);

            _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x() + Defines::gridLength,
                               _currentPoint.y());
            _currentPoint.setX(_currentPoint.x() + Defines::gridLength);
            _diffX -= Defines::gridLength;
            if (!isThereAComponentOrADescription(_currentPoint.x() + (Defines::gridLength / 2),
                                                 _currentPoint.y() - (Defines::gridLength / 2)) || _diffX == 0)
            {
                QRect hitbox(_currentPoint.x() - 5, _currentPoint.y() - (Defines::gridLength / 2), 10, (Defines::gridLength / 2));
                _connectionHitbox.append(hitbox);

                _painter->drawLine(_currentPoint.x(), _currentPoint.y(), _currentPoint.x(), _currentPoint.y() - (Defines::gridLength / 2));
                _currentPoint.setY(_currentPoint.y() - (Defines::gridLength / 2));
            }
            else
            {
                dodgeComponent();
            }
        }
    }
}

void Connection::initializeValues(void)
{
    _countChangeDirection = 0;

    _diffX = _endPoint.x() - _startPoint.x();
    _diffY = _endPoint.y() - _startPoint.y();
    _currentPoint = _startPoint;
}

bool Connection::operator==(const Connection &rhs)
{
    bool equalInEqualDirection = (_componentPortOne == rhs.getComponentPortOne() &&
                                  _componentPortTwo == rhs.getComponentPortTwo());
    bool equalInOtherDirection = (_componentPortOne == rhs.getComponentPortTwo() &&
                                  _componentPortTwo == rhs.getComponentPortOne());
    return equalInEqualDirection || equalInOtherDirection;
}

bool Connection::operator!=(const Connection &rhs)
{
    return !(operator==(rhs));
}

int Connection::horizontalPathAnalysis(int howManyConnections, bool horizontalFirst)
{
    if (_diffX > 0)
    {
        howManyConnections = leftOrRightPathAnalysis(howManyConnections, horizontalFirst, (Defines::gridLength / 2));
    }
    else if (_diffX < 0)
    {
        howManyConnections = leftOrRightPathAnalysis(howManyConnections, horizontalFirst, -(Defines::gridLength / 2));
    }
    if (horizontalFirst)
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
    while (_diffX != 0)
    {
        if (!isThereAComponentOrADescription(_currentPoint.x() + leftOrRight, _currentPoint.y()))
        {
            howManyConnections += 1;
            _currentPoint.setX(_currentPoint.x() + leftOrRight);
            _diffX = _diffX - leftOrRight;
            _isDodgedBeforePathAnalyse = false;
        }
        else
        {
            if (_diffY == 0)
            {
                howManyConnections = pathAnalysisDodgeComponent(howManyConnections);
            }
            else
            {
                _countChangeDirection += 1;
                howManyConnections = verticalPathAnalysis(howManyConnections, horizontalFirst);
            }
        }
    }
    return howManyConnections;
}

int Connection::verticalPathAnalysis(int howManyConnections, bool horizontalFirst)
{
    if (_diffY > 0)
    {
        howManyConnections = upOrDownPathAnalysis(howManyConnections, horizontalFirst, (Defines::gridLength / 2));
    }
    else if (_diffY < 0)
    {
        howManyConnections = upOrDownPathAnalysis(howManyConnections, horizontalFirst, -(Defines::gridLength / 2));
    }
    if (!horizontalFirst)
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
    while (_diffY != 0)
    {
        if (!isThereAComponentOrADescription(_currentPoint.x(), _currentPoint.y() + upOrDown))
        {
            howManyConnections += 1;
            _currentPoint.setY(_currentPoint.y() + upOrDown);
            _diffY = _diffY - upOrDown;
            _isDodgedBeforePathAnalyse = false;
        }
        else
        {
            if (_diffX == 0)
            {
                howManyConnections = pathAnalysisDodgeComponent(howManyConnections);
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

int Connection::pathAnalysisDodgeComponent(int howManyConnections)
{
    if (_diffX == 0)
    {
        if (!_isDodgedBeforePathAnalyse)
        {
            howManyConnections += 1;
            _currentPoint.setX(_currentPoint.x() + (Defines::gridLength / 2));
            _isDodgedBeforePathAnalyse = true;
        }
        if (_diffY < 0)
        {
            howManyConnections += 2;
            _currentPoint.setY(_currentPoint.y() - Defines::gridLength);
            _diffY += Defines::gridLength;
            if (!isThereAComponentOrADescription(_currentPoint.x() - (Defines::gridLength / 2),
                                                 _currentPoint.y() - (Defines::gridLength / 2)) || _diffY == 0)
            {
                howManyConnections += 1;
                _currentPoint.setX(_currentPoint.x() - (Defines::gridLength / 2));
            }
            else
            {
                howManyConnections = pathAnalysisDodgeComponent(howManyConnections);
            }
        }
        if (_diffY > 0)
        {
            howManyConnections += 2;
            _currentPoint.setY(_currentPoint.y() + Defines::gridLength);
            _diffY -= Defines::gridLength;
            if (!isThereAComponentOrADescription(_currentPoint.x() - (Defines::gridLength / 2),
                                                 _currentPoint.y() + (Defines::gridLength / 2)) || _diffY == 0)
            {
                howManyConnections += 1;
                _currentPoint.setX(_currentPoint.x() - (Defines::gridLength / 2));
            }
            else
            {
                howManyConnections = pathAnalysisDodgeComponent(howManyConnections);
            }
        }
    }
    if (_diffY == 0)
    {
        if (!_isDodgedBeforePathAnalyse)
        {
            howManyConnections += 1;
            _currentPoint.setY(_currentPoint.y() + (Defines::gridLength / 2));
            _isDodgedBeforePathAnalyse = true;
        }
        if (_diffX < 0)
        {
            howManyConnections += 2;
            _currentPoint.setX(_currentPoint.x() - Defines::gridLength);
            _diffX += Defines::gridLength;
            if (!isThereAComponentOrADescription(_currentPoint.x() - (Defines::gridLength / 2),
                                                 _currentPoint.y() - (Defines::gridLength / 2)) || _diffX == 0)
            {
                howManyConnections += 1;
                _currentPoint.setY(_currentPoint.y() - (Defines::gridLength / 2));
            }
            else
            {
                howManyConnections = pathAnalysisDodgeComponent(howManyConnections);
            }
        }
        if (_diffX > 0)
        {
            howManyConnections += 2;
            _currentPoint.setX(_currentPoint.x() + Defines::gridLength);
            _diffX -= Defines::gridLength;
            if (!isThereAComponentOrADescription(_currentPoint.x() + (Defines::gridLength / 2),
                                                 _currentPoint.y() - (Defines::gridLength / 2)) || _diffX == 0)
            {
                howManyConnections += 1;
                _currentPoint.setY(_currentPoint.y() - (Defines::gridLength / 2));
            }
            else
            {
                howManyConnections = pathAnalysisDodgeComponent(howManyConnections);
            }
        }
    }
    return howManyConnections;
}

int Connection::signum(int checkedNumber)
{
    if (checkedNumber < 0)
    {
        return -1;
    }
    else if (checkedNumber > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void Connection::paintStartAndEndPoint(QPainter* painter)
{
    //Punkte an Enden zeichnen
    painter->setBrush(QBrush(Qt::black));
    painter->drawEllipse(_startPoint.toPoint(), _circleRadius, _circleRadius);
    painter->drawEllipse(_endPoint.toPoint(), _circleRadius, _circleRadius);
}
