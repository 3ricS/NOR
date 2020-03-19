#include "defines.h"
#include "component.h"

#include <QDebug>

Component::Component(int x, int y, bool isVertical, QString name, double voltage, ComponentType componentTyp, int id)
        : GridObject(QPointF(x, y), id),
        _isVertical(isVertical), _name(name), _voltage(voltage),
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
    int xPosition = _position.x();
    int yPosition = _position.y();
    return QRectF(xPosition - (Defines::gridLength / 2), yPosition - (Defines::gridLength / 2), Defines::gridLength,
                  Defines::gridLength);
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
    int xPosition = _position.x();
    int yPosition = _position.y();
    bool xEqualPortA = (position.x() > getPortPositionXOrY(xPosition, Port::A, true) - _hitBoxSize &&
                        position.x() < getPortPositionXOrY(xPosition, Port::A, true) + _hitBoxSize);
    bool yEqualPortA = (position.y() > getPortPositionXOrY(yPosition, Port::A, false) - _hitBoxSize &&
                        position.y() < getPortPositionXOrY(yPosition, Port::A, false) + _hitBoxSize);
    bool xEqualPortB = (position.x() > getPortPositionXOrY(xPosition, Port::B, true) - _hitBoxSize &&
                        position.x() < getPortPositionXOrY(xPosition, Port::B, true) + _hitBoxSize);
    bool yEqualPortB = (position.y() > getPortPositionXOrY(yPosition, Port::B, false) - _hitBoxSize &&
                        position.y() < getPortPositionXOrY(yPosition, Port::B, false) + _hitBoxSize);

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
void Component::paintHighlightRect(QPainter* painter)
{
    QBrush brush;
    brush.setColor(QColor(255, 0, 0, 55));
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    painter->setBrush(brush);
    int xPosition = _position.x();
    int yPosition = _position.y();
    painter->drawRect(xPosition - (Defines::gridLength / 2), yPosition - (Defines::gridLength / 2),
                      Defines::gridLength, Defines::gridLength);
}

QPointF Component::getPortPosition(Component::Port port) const
{
    int xPosition = _position.x();
    int yPosition = _position.y();
    return QPointF(getPortPositionXOrY(xPosition, port, true), getPortPositionXOrY(yPosition, port, false));
}

void Component::setOrientation(Component::Orientation newOrientation)
{
    _orientation = newOrientation;
    _isVertical = !(Orientation::left == newOrientation || Orientation::right == newOrientation);
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
