#include "defines.h"
#include "component.h"

#include <QDebug>

Component::Component(QPointF position, bool isVertical, QString name, double voltage, ComponentType componentTyp,
                     int id)
        : GridObject(position, id),
          _name(name), _voltage(voltage),
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

    if ((isVertical() && !isX) || (!isVertical() && isX))
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
    int xPosition = _position.x();
    int yPosition = _position.y();
    bool xEqualPortA = (position.x() > getPortPositionXOrY(xPosition, Port::A, true) - _hitBoxSizeAtPort &&
                        position.x() < getPortPositionXOrY(xPosition, Port::A, true) + _hitBoxSizeAtPort);
    bool yEqualPortA = (position.y() > getPortPositionXOrY(yPosition, Port::A, false) - _hitBoxSizeAtPort &&
                        position.y() < getPortPositionXOrY(yPosition, Port::A, false) + _hitBoxSizeAtPort);
    bool xEqualPortB = (position.x() > getPortPositionXOrY(xPosition, Port::B, true) - _hitBoxSizeAtPort &&
                        position.x() < getPortPositionXOrY(xPosition, Port::B, true) + _hitBoxSizeAtPort);
    bool yEqualPortB = (position.y() > getPortPositionXOrY(yPosition, Port::B, false) - _hitBoxSizeAtPort &&
                        position.y() < getPortPositionXOrY(yPosition, Port::B, false) + _hitBoxSizeAtPort);

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
 * \param   position ist die Position, an dem ein Port gesucht werden soll
 * \return  Gibt an, ob sich an der zu prüfenden Position ein Port befindet
 *
 * Es wird geprüft, ob der Port an der Position ungleich dem nullptr ist und somit an der Position ein Port vorhanden ist.
 */
bool Component::hasPortAtPosition(QPointF position) const
{
    Port foundPort = getPort(position);
    return Port::null != foundPort;
}

/*!
 * \brief Wandelt den Integer in einen Komponententyp.
 *
 * \param   componentType ist eine Zahl (int), der für einen Komponententyp steht
 * \return  Gibt den ComponentType der zu geprüften Komponente zurück
 *
 * Es wird geprüft ob es sich bei der Komponente componentType um einen Widerstand oder eine Spannungsquelle handelt.
 */
Component::ComponentType Component::convertToComponentType(int componentType)
{
    Component::ComponentType type = ComponentType::PowerSupply;
    if (0 == componentType)
    {
        type = ComponentType::Resistor;
    }

    return type;
}

/*!
 * \brief Liefert die Position eines bestimmten Ports zurück
 *
 * \return Position den gesuchten Ports
 */
QPointF Component::getPortPosition(Component::Port port) const
{
    int xPosition = _position.x();
    int yPosition = _position.y();
    return QPointF(getPortPositionXOrY(xPosition, port, true), getPortPositionXOrY(yPosition, port, false));
}

void
Component::paintInformation(QPainter* painter, QString name, double value, QRectF namePosition, QRectF valuePosition,
                            ComponentType componentType)
{
    QFont q;
    q.setPixelSize(13);
    painter->setFont(q);

    QString displayedValueString = getDisplayedValueString(value, componentType);

    painter->drawText(namePosition, Qt::AlignLeft, name);
    painter->drawText(valuePosition, Qt::AlignRight, displayedValueString);
}

QString Component::getDisplayedValueString(double value, Component::ComponentType componentType)
{
    double valueWithoutUnit = value;
    QString unitString = getScaledValue(valueWithoutUnit);

    if (ComponentType::Resistor == componentType)
    {
        unitString.append("Ω");
    }
    else
    {
        unitString.append("V");
    }

    QString valueWithLocalApperance = QLocale::system().toString(valueWithoutUnit);
    valueWithLocalApperance.replace(".", "");
    QString displayedString = valueWithLocalApperance + " " + unitString;

    return displayedString;
}

QString Component::getScaledValue(double& valueWithoutUnit)
{
    QString unitString = "";
    double value = valueWithoutUnit;

    if (value < 1e3)
    {
        valueWithoutUnit = value;
    }
    else if (value < 1e6)
    {
        valueWithoutUnit = value / 1e3;
        unitString = "k";
    }
    else if (value < 1e9)
    {
        unitString = "M";
        valueWithoutUnit = value / 1e6;
    }
    else
    {
        unitString = "G";
        valueWithoutUnit = value / 1e9;
    }

    valueWithoutUnit = int(valueWithoutUnit * 100) / 100.0;
    return unitString;
}
/*!
 * \brief Gibt an, ob die Komponente vertikal ausgerichtet ist
 *
 * \return Liefert true, wenn die Komponente vertikal ausgerichtet ist.
 */
bool Component::isVertical() const
{
    return (Orientation::top == _orientation || Orientation::bottom == _orientation);
}

bool Component::operator==(const Component &rhs)
{
    bool equalType = (_componentType == rhs._componentType);
    bool equalName = (_name == rhs._name);
    bool equalOrientation = (_orientation == rhs._orientation);
    bool equalVoltage = (_voltage == rhs._voltage);
    bool equalCurrent = (_amp == rhs._amp);
    bool equalPosition = (_position == rhs._position);
    bool equalId = (_id == rhs._id);
    bool equal = equalType && equalName && equalOrientation &&
            equalVoltage && equalCurrent && equalPosition && equalId;
    return equal;
}

bool Component::operator!=(const Component &rhs)
{
    return !(operator==(rhs));
}
