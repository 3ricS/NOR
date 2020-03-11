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
Resistor::Resistor(QString name, int valueResistance, int x, int y, bool isVertical, int id)
        : Component(x, y, isVertical, name, valueResistance, Component::ComponentType::Resistor, id)
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
        painter->drawRect(_xPosition - 20, _yPosition - 30, 40, 60);
        painter->drawLine(_xPosition, _yPosition - 30, _xPosition, _yPosition - Defines::halfGridLength + Connection::_circleRadius);
        painter->drawLine(_xPosition, _yPosition + 30, _xPosition, _yPosition + Defines::halfGridLength - Connection::_circleRadius);
    }
    else
    {
        painter->drawRect(_xPosition - 30, _yPosition - 20, 60, 40);
        painter->drawLine(_xPosition - 30, _yPosition + 0, _xPosition - Defines::halfGridLength + Connection::_circleRadius, _yPosition + 0);
        painter->drawLine(_xPosition + 30, _yPosition + 0, _xPosition + Defines::halfGridLength - Connection::_circleRadius, _yPosition + 0);
    }
    if(_isSelected)
    {
        paintHighlightRect(painter);
    }

    //Zeichnen der Informationen (Name und Wert) in Abhängigkeit ob vertikal oder horizontal
    paintInformations(painter);
}

void Resistor::paintInformation(QPainter* painter)
{
    if(_isVertical)
    {
        paintOrientationSensitiv(painter, _xPosition - 40, _yPosition - 30, _xPosition - 60, _yPosition);
    }
    else
    {
        paintOrientationSensitiv(painter, _xPosition - 5, _yPosition - 25, _xPosition - 5, _yPosition + 40);
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
    if(_value < 1000)
    {
        painter->drawText(xPosValue, yPosValue, QString::number(_value) + "Ω");
    }
    else
    {
        painter->drawText(xPosValue, yPosValue, QString::number((double)_value / 1000) + "kΩ");
    }
}
