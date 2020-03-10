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
PowerSupply::PowerSupply(QString name, int x, int y, bool isVertical, int id) :
        Component(x, y, isVertical, name, 0, Component::ComponentType::PowerSupply, id)
{
    //Default Spannung
    _voltage = 100.0;
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

    if (_isVertical)
    {
        painter->drawEllipse(_xPosition - 30, _yPosition - 30, 60, 60);
        painter->drawLine(_xPosition, _yPosition + Defines::halfGridLength, _xPosition, _yPosition - Defines::halfGridLength);
    }
    else
    {
        painter->drawEllipse(_xPosition - 30, _yPosition - 30, 60, 60);
        painter->drawLine(_xPosition - Defines::halfGridLength, _yPosition, _xPosition + Defines::halfGridLength, _yPosition);
    }
    if(_isSelected)
    {
        paintHighlightRect(painter);
    }

    paintInformations(painter);
}

void PowerSupply::paintInformations(QPainter* painter)
{
    QFont q;
    q.setPixelSize(13);
    painter->setFont(q);
    painter->drawText(_xPosition - 40, _yPosition - 30, _name);
    if(_voltage > 1000)
    {
        painter->drawText(_xPosition + 10, _yPosition - 30, QString::number(_voltage / 1000) + "kV");
    }
    else
    {
        painter->drawText(_xPosition + 10, _yPosition - 30, QString::number(_voltage) + "V");
    }
}
