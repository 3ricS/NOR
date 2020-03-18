#include "defines.h"
#include "component.h"

#include <QDebug>

Component::Component(int x, int y, bool isVertical, QString name, double voltage, ComponentType componentTyp, int id)
        : QGraphicsItem(nullptr),
          _xPosition(x), _yPosition(y), _isVertical(isVertical),
          _name(name), _voltage(voltage), _id(id),
          _componentType(componentTyp)
{
    if (isVertical)
    {
        _orientation = Orientation::top;
    }
    else
    {
        _orientation = Orientation::left;
    }
}

/*!
 * \brief
 *
 * \return
 */
QRectF Component::boundingRect(void) const
{
    return QRectF(_xPosition - (Defines::gridLength / 2), _yPosition - (Defines::gridLength / 2), Defines::gridLength, Defines::gridLength);
}

int Component::getPortPositionXOrY(int positionValue, Port port, bool isX) const
{
    //Portangabe in Faktor umrechnen
    int factor = 0;
    switch (port)
    {
        case A:
            factor = -1;
            break;
        case B:
            factor = 1;
            break;
        case null:
        {
        }
            break;
    }

    if ((_isVertical && !isX) || (!_isVertical && isX))
    {
        return positionValue + factor * Defines::gridLength / 2;
    }
    else
    {
        return positionValue;
    }
}

/*!
 * \brief Liefert den Port der Komponente.
 *
 * \param   position ist die zu prüfende Position
 * \return  Gibt den Port der Komponente an der Position zurück
 */
Component::Port Component::getPort(QPointF position) const
{
    bool xEqualPortA = (position.x() > getPortPositionXOrY(_xPosition, Port::A, true) - _hitBoxSize &&
                        position.x() < getPortPositionXOrY(_xPosition, Port::A, true) + _hitBoxSize);
    bool yEqualPortA = (position.y() > getPortPositionXOrY(_yPosition, Port::A, false) - _hitBoxSize &&
                        position.y() < getPortPositionXOrY(_yPosition, Port::A, false) + _hitBoxSize);
    bool xEqualPortB = (position.x() > getPortPositionXOrY(_xPosition, Port::B, true) - _hitBoxSize &&
                        position.x() < getPortPositionXOrY(_xPosition, Port::B, true) + _hitBoxSize);
    bool yEqualPortB = (position.y() > getPortPositionXOrY(_yPosition, Port::B, false) - _hitBoxSize &&
                        position.y() < getPortPositionXOrY(_yPosition, Port::B, false) + _hitBoxSize);

    if (xEqualPortA && yEqualPortA)
    {
        return Port::A;
    }
    else if (xEqualPortB && yEqualPortB)
    {
        return Port::B;
    }
    else
    {
        return Port::null;
    }
}

/*!
 * \brief Prüft ob sich an der ausgewählten Position ein Port befindet.
 *
 * \param   position ist die zu prüfende Position
 * \return  Liefert einen bool zurück, ob sich an der zu prüfenden Position ein Port befindet
 *
 * Es wird geprüft, ob der Port an der Position != dem nullptr ist und somit an der Position ein Port vorhanden ist.
 */
bool Component::hasPortAtPosition(QPointF position) const
{
    Port foundPort = getPort(position);
    return Port::null != foundPort;
}

/*!
 * \brief Wandelt den Integer in einen Komponententyp.
 *
 * \param   componentType ist ein int, der für einen Komponententyp steht
 * \return  Gibt den ComponentType der zu prüfenden Komponente zurück
 *
 * Es wird geprüft ob es sich bei dem componentType um einen Widerstand oder eine Spannungsquelle handelt.
 */
Component::ComponentType Component::integerToComponentType(int componentType)
{
    Component::ComponentType type = ComponentType::PowerSupply;
    if (0 == componentType)
    {
        type = ComponentType::Resistor;
    }

    return type;
}

/*!
 * \brief Highlightet das Textfeld in einer Farbe.
 *
 * \param   painter
 *
 * Färbt das Gitterfeld um die Komponente an der ausgewählten Position, mit der eingestelten Farbe, ein.
 */
void Component::paintHighlightRect(QPainter *painter)
{
    QBrush brush;
    brush.setColor(QColor(255, 0, 0, 55));
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    painter->setBrush(brush);
    painter->drawRect(_xPosition - (Defines::gridLength / 2), _yPosition - (Defines::gridLength / 2), Defines::gridLength, Defines::gridLength);
}

QPointF Component::getPortPosition(Component::Port port) const
{
    return QPointF(getPortPositionXOrY(_xPosition, port, true), getPortPositionXOrY(_yPosition, port, false));
}

void Component::setPosition(QPointF gridPosition)
{
    _xPosition = gridPosition.toPoint().x();
    _yPosition = gridPosition.toPoint().y();
}

void Component::setSelected(bool isSelected)
{
    _isSelected = isSelected;

}

void Component::setOrientation(Component::Orientation newOrientation)
{
    _orientation = newOrientation;
    _isVertical = !(Orientation::left == newOrientation || Orientation::right == newOrientation);
}
