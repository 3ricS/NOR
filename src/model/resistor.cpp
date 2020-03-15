#include "defines.h"
#include "resistor.h"
#include "connection.h"

#include <QLocale>

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
Resistor::Resistor(QString name, long double valueResistance, int x, int y, bool isVertical, int id)
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
void Resistor::paint(QPainter* painter, [[maybe_unused]] const QStyleOptionGraphicsItem* option,
                     [[maybe_unused]] QWidget* widget)
{

    if(_isVertical)
    {
        //Resistor has length of 120 and width of 60
        painter->drawRect(_xPosition - (Defines::gridLength * 0.2), _yPosition - (Defines::gridLength * 0.3),
                          (Defines::gridLength * 0.4), Defines::gridLength * 0.6);
        painter->drawLine(_xPosition, _yPosition - (Defines::gridLength * 0.3), _xPosition,
                          _yPosition - (Defines::gridLength / 2) + Connection::_circleRadius);
        painter->drawLine(_xPosition, _yPosition + (Defines::gridLength * 0.3), _xPosition,
                          _yPosition + (Defines::gridLength / 2) - Connection::_circleRadius);
    }
    else
    {
        painter->drawRect(_xPosition - (Defines::gridLength * 0.3), _yPosition - (Defines::gridLength * 0.2),
                          (Defines::gridLength * 0.6), Defines::gridLength * 0.4);
        painter->drawLine(_xPosition - (Defines::gridLength * 0.3), _yPosition + 0,
                          _xPosition - (Defines::gridLength / 2) + Connection::_circleRadius, _yPosition + 0);
        painter->drawLine(_xPosition + (Defines::gridLength * 0.3), _yPosition + 0,
                          _xPosition + (Defines::gridLength / 2) - Connection::_circleRadius, _yPosition + 0);
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
    // Zeichnen des Namens
    QFont q;
    q.setPixelSize(13);
    painter->setFont(q);

    QRectF posText;
    QRectF posValue;
    if(_isVertical)
    {
        posText = QRectF(QPointF(_xPosition - (Defines::gridLength * 0.2), _yPosition - (Defines::gridLength * 0.45)),
                       QSizeF(Defines::gridLength * 0.7, Defines::gridLength * 0.2));
        posValue = QRectF(QPointF(_xPosition - (Defines::gridLength * 0.5), _yPosition + (Defines::gridLength * 0.315)),
                          QSizeF(Defines::gridLength * 0.9, Defines::gridLength * 0.2));;
    }
    else
    {
        posText = QRectF(QPointF(_xPosition - (Defines::gridLength * 0.3), _yPosition - (Defines::gridLength * 0.35)),
                QSizeF(Defines::gridLength * 0.6, Defines::gridLength * 0.2));
        posValue = QRectF(QPointF(_xPosition - (Defines::gridLength * 0.3), _yPosition + (Defines::gridLength * 0.215)),
                        QSizeF(Defines::gridLength * 0.6, Defines::gridLength * 0.2));
    }

    painter->drawText(posText, Qt::AlignLeft, _name);

    double resistance = static_cast<double>(_resistanceValue);
    double resistanceWithoutUnit;
    QString unitString = "";
    // Darstellung des Widerstandwertes
    if(resistance < 1000)
    {
        resistanceWithoutUnit = resistance;
    }
    else if(resistance < 1000000)
    {
        resistanceWithoutUnit = resistance / 1000;
        unitString = "k";
    }
    else
    {
        unitString = "M";
        resistanceWithoutUnit = resistance / 1000000;
    }
    resistanceWithoutUnit = int(resistanceWithoutUnit * 100) / 100.0;
    QString valueString = QLocale::system().toString(resistanceWithoutUnit);
    valueString.replace(".", "");
    QString displayString = valueString + " " + unitString + "Ω";
    painter->drawText(posValue, Qt::AlignRight, displayString);
}
