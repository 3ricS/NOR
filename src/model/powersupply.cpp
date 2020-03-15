#include "defines.h"
#include "powersupply.h"
#include "connection.h"

#include <QLocale>

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
void PowerSupply::paint(QPainter* painter, [[maybe_unused]] const QStyleOptionGraphicsItem* option,
                        [[maybe_unused]] QWidget* widget)
{
    painter->drawEllipse(_xPosition - (Defines::gridLength * 0.3), _yPosition - (Defines::gridLength * 0.3),
                         Defines::gridLength * 0.6, Defines::gridLength * 0.6);
    if(_isVertical)
    {
        painter->drawLine(_xPosition, _yPosition + (Defines::gridLength / 2) - Connection::_circleRadius,
                _xPosition,_yPosition - (Defines::gridLength / 2) + Connection::_circleRadius);
    }
    else
    {
        painter->drawLine(_xPosition - (Defines::gridLength / 2) + Connection::_circleRadius, _yPosition,
                          _xPosition + (Defines::gridLength / 2) - Connection::_circleRadius, _yPosition);
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

    QRectF posName(QPointF(_xPosition - (Defines::gridLength * 0.3), _yPosition - (Defines::gridLength * 0.43)),
                   QSize(Defines::gridLength * 0.7, 20));
    painter->drawText(posName, Qt::AlignLeft, _name);
    double voltageWithoutUnit;
    QString unitString = "";
    // Darstellung des Widerstandwertes
    if(_voltage < 1000)
    {
        voltageWithoutUnit = _voltage;
    }
    else if(_voltage < 1000000)
    {
        voltageWithoutUnit = _voltage / 1000;
        unitString = "k";
    }
    else
    {
        unitString = "M";
        voltageWithoutUnit = _voltage / 1000000;
    }
    voltageWithoutUnit = int(voltageWithoutUnit * 100) / 100.0;
    QString valueString = QLocale::system().toString(voltageWithoutUnit);
    valueString.replace(".", "");
    QString displayString = valueString + " " + unitString + "Ω";
    QRectF posValue;
    if(_isVertical)
    {
        posValue = QRectF(QPointF(_xPosition - (Defines::gridLength * 0.4), _yPosition + (Defines::gridLength * 0.32)),
                          QSize(Defines::gridLength * 0.8, 20));
    }
    else
    {
        posValue = QRectF(QPointF(_xPosition - (Defines::gridLength * 0.4), _yPosition + (Defines::gridLength * 0.32)),
                          QSize(Defines::gridLength * 0.7, 20));
    }
    painter->drawText(posValue, Qt::AlignRight, displayString);
}
