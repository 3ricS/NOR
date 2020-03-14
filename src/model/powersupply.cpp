#include "defines.h"
#include "powersupply.h"
/*!
 * \brief Erzeugt ein neues Spannungsquellenobjekt
 *
 * \param   name            ist der Name des Spannungsquellenobjektes
 * \param   x               ist die X-Koordinate der zugewiesenen Position
 * \param   y               ist die Y-Koordinate der zugewiesenen Position
 * \param   isVertical      ist die räumliche Ausrichtung im Netzwerk
 * \param   id              ist die intern zugewiesene Id
 *
 */
PowerSupply::PowerSupply(QString name, int x, int y, bool isVertical, double voltage, int id) :
        Component(x, y, isVertical, name, voltage, Component::ComponentType::PowerSupply, id)
{
}

/*!
 * \brief Zeichnet die Umrisse und die Informationen einer Spannungsquelle im Netzwerk.
 *
 * \param   painter
 * \param   option
 * \param   widget
 *
 * Die Methode zeichnet abhängig von der räumlichen Ausrichtung die Spannungsquelle auf zwei Weisen.
 */
void PowerSupply::paint(QPainter* painter, [[maybe_unused]] const QStyleOptionGraphicsItem* option, [[maybe_unused]] QWidget* widget)
{
    painter->drawEllipse(_xPosition - (Defines::gridLength * 0.3), _yPosition - (Defines::gridLength * 0.3), Defines::gridLength * 0.6, Defines::gridLength * 0.6);
    if (_isVertical)
    {
        painter->drawLine(_xPosition, _yPosition + (Defines::gridLength / 2), _xPosition, _yPosition - (Defines::gridLength / 2));
    }
    else
    {
        painter->drawLine(_xPosition - (Defines::gridLength / 2), _yPosition, _xPosition + (Defines::gridLength / 2), _yPosition);
    }
    if(_isSelected)
    {
        paintHighlightRect(painter);
    }

    paintInformation(painter);
}

void PowerSupply::paintInformation(QPainter* painter)
{
    QFont q;
    q.setPixelSize(13);
    painter->setFont(q);
    painter->drawText(_xPosition - (Defines::gridLength * 0.4), _yPosition - (Defines::gridLength * 0.3), _name);
    if(_voltage > 1000)
    {
        painter->drawText(_xPosition + (Defines::gridLength * 0.1), _yPosition - (Defines::gridLength * 0.3), QString::number(_voltage / 1000) + "kV");
    }
    else
    {
        painter->drawText(_xPosition + (Defines::gridLength * 0.1), _yPosition - (Defines::gridLength * 0.3), QString::number(_voltage) + "V");
    }
}
