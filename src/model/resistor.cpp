#include "defines.h"
#include "resistor.h"
#include "connection.h"

/*!
 * \brief Erzeugt ein neues Widerstandsobjekt
 *
 * \param   name            ist der Name des Widerstandsobjektes
 * \param   valueResistance ist der zugewiesene Widerstandswertes
 * \param   x               ist die X-Koordinate der zugewiesenen Position
 * \param   y               ist die Y-Koordinate der zugewiesenen Position
 * \param   isVertical      ist die räumliche Ausrichtung im Netzwerk
 * \param   id              ist die intern zugewiesene Id
 *
 */
Resistor::Resistor(QString name, double valueResistance, int x, int y, bool isVertical, int id)
        : Component(x, y, isVertical, name, 0.0, Component::ComponentType::Resistor, id),
        _resistanceValue(valueResistance)
{
}


/*!
 * \brief Zeichnet die Umrisse und Informationen eines Widerstandes im Netzwerk.
 *
 * \param   painter
 * \param   option
 * \param   widget
 *
 * Die Methode zeichnet abhängig von der räumlichen Ausrichtung den Widerstand au zwei unterschiedliche Weisen.
 */
void Resistor::paint(QPainter* painter, [[maybe_unused]] const QStyleOptionGraphicsItem* option, [[maybe_unused]] QWidget* widget)
{

    if(_isVertical)
    {
        //Resistor has length of 120 and width of 60
        painter->drawRect(_xPosition - (Defines::gridLength * 0.2), _yPosition - (Defines::gridLength * 0.3), (Defines::gridLength * 0.4), Defines::gridLength * 0.6);
        painter->drawLine(_xPosition, _yPosition - (Defines::gridLength * 0.3), _xPosition, _yPosition - (Defines::gridLength / 2) + Connection::_circleRadius);
        painter->drawLine(_xPosition, _yPosition + (Defines::gridLength * 0.3), _xPosition, _yPosition + (Defines::gridLength / 2) - Connection::_circleRadius);
    }
    else
    {
        painter->drawRect(_xPosition - (Defines::gridLength * 0.3), _yPosition - (Defines::gridLength * 0.2), (Defines::gridLength * 0.6), Defines::gridLength * 0.4);
        painter->drawLine(_xPosition - (Defines::gridLength * 0.3), _yPosition + 0, _xPosition - (Defines::gridLength / 2) + Connection::_circleRadius, _yPosition + 0);
        painter->drawLine(_xPosition + (Defines::gridLength * 0.3), _yPosition + 0, _xPosition + (Defines::gridLength / 2) - Connection::_circleRadius, _yPosition + 0);
    }
    if(_isSelected)
    {
        paintHighlightRect(painter);
    }

    //Zeichnen der Informationen (Name und Wert) in Abhängigkeit ob vertikal oder horizontal
    paintInformation(painter);
}

void Resistor::paintInformation(QPainter* painter)
{
    if(_isVertical)
    {
        paintOrientationSensitiv(painter, _xPosition - (Defines::gridLength * 0.4), _yPosition - (Defines::gridLength * 0.32), _xPosition - (Defines::gridLength * 0.45), _yPosition);
    }
    else
    {
        paintOrientationSensitiv(painter, _xPosition - (Defines::gridLength * 0.05), _yPosition - (Defines::gridLength * 0.25), _xPosition - (Defines::gridLength * 0.05), _yPosition + (Defines::gridLength * 0.4));
    }
}

void Resistor::paintOrientationSensitiv(QPainter* painter, int xPosText, int yPosText, int xPosValue, int yPosValue)
{
    // Zeichnen des Namens
    QFont q;
    q.setPixelSize(13);
    painter->setFont(q);
    painter->drawText(xPosText, yPosText, _name);

    // Darstellung des Widerstandwertes
    if(_resistanceValue < 1000)
    {
        painter->drawText(xPosValue, yPosValue, QString::number(_resistanceValue) + "Ω");
    }
    else if(_resistanceValue < 1000000)
    {
        painter->drawText(xPosValue, yPosValue, QString::number(_resistanceValue / 1000) + "kΩ");
    }
    else
    {
        painter->drawText(xPosValue, yPosValue, QString::number(_resistanceValue / 1000000) + "MΩ");
    }

}
