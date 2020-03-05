#include "defines.h"
#include "powersupply.h"
/*!
 * \brief Erzeugt ein neues Spannungsquellenobjekt
 *
 * \param[in]    name            ist der Name des Spannungsquellenobjektes
 * \param[in]    x               ist die X-Koordinate der zugewiesenen Position
 * \param[in]    y               ist die Y-Koordinate der zugewiesenen Position
 * \param[in]    isVertical      ist die räumliche Ausrichtung im Netzwerk
 * \param[in]    id              ist die intern zugewiesene Id
 *
 */
PowerSupply::PowerSupply(QString name, int x, int y, bool isVertical, int id) :
        Component(x, y, isVertical, name, 0, Component::ComponentType::PowerSupply, id)
{
}

/*!
 * \brief Zeichnet die Umrisse und die Informationen einer Spannungsquelle im Netzwerk.
 *
 * \param[in]    painter
 * \param[]      option
 * \param[]      widget
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
    painter->drawText(_xPosition - 40, _yPosition - 40, _name);
}
