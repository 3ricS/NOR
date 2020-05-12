#include "defines.h"
#include "resistor.h"
#include "connection.h"
#include "model/networkgraphics.h"

#include <QLocale>

Resistor::Resistor(QString name, long double valueResistance, int x, int y, bool isVertical, int id, NetworkGraphics* model)
        : Component(QPointF(x, y), isVertical, name, 0.0, Component::ComponentType::Resistor, id, model),
          _resistanceValue(valueResistance)
{
}

void Resistor::paint(QPainter* painter, [[maybe_unused]] const QStyleOptionGraphicsItem* option,
                     [[maybe_unused]] QWidget* widget)
{
    int xPosition = _position.x();
    int yPosition = _position.y();

    int deviationPortA = 0;
    int deviationPortB = 0;
    calculateDeviationPortAB(deviationPortA, deviationPortB);

    if(isVertical())
    {
        //Resistor has length of 120 and width of 60
        painter->drawRect(xPosition - (Defines::gridLength * 0.2), yPosition - (Defines::gridLength * Defines::gridObjectWidth / 2),
                          (Defines::gridLength * 0.4), Defines::gridLength * Defines::gridObjectWidth);
        painter->drawLine(xPosition, yPosition - (Defines::gridLength * Defines::gridObjectWidth / 2), xPosition,
                          yPosition - (Defines::gridLength / 2) + Connection::_circleRadius + deviationPortA);
        painter->drawLine(xPosition, yPosition + (Defines::gridLength * Defines::gridObjectWidth / 2), xPosition,
                          yPosition + (Defines::gridLength / 2) - Connection::_circleRadius - deviationPortB);
    }
    else
    {
        painter->drawRect(xPosition - (Defines::gridLength * Defines::gridObjectWidth / 2), yPosition - (Defines::gridLength * 0.2),
                          (Defines::gridLength * Defines::gridObjectWidth), Defines::gridLength * 0.4);
        painter->drawLine(xPosition - (Defines::gridLength * Defines::gridObjectWidth / 2), yPosition + 0,
                          xPosition - (Defines::gridLength / 2) + Connection::_circleRadius + deviationPortA, yPosition + 0);
        painter->drawLine(xPosition + (Defines::gridLength * Defines::gridObjectWidth / 2), yPosition + 0,
                          xPosition + (Defines::gridLength / 2) - Connection::_circleRadius - deviationPortB, yPosition + 0);
    }

    if(_isSelected)
    {
        paintSelectionHighlight(painter);
    }

    //Zeichnen der Informationen (Name und Wert) in Abhängigkeit ob vertikal oder horizontal
    setLabelPositions(painter);
}

void Resistor::setLabelPositions(QPainter* painter)
{
    int xPosition = _position.x();
    int yPosition = _position.y();

    QRectF posText;
    QRectF posValue;
    if(isVertical())
    {
        posText = QRectF(QPointF(xPosition - (Defines::gridLength * 0.2), yPosition - (Defines::gridLength * 0.45)),
                       QSizeF(Defines::gridLength * 0.7, Defines::gridLength * 0.2));
        posValue = QRectF(QPointF(xPosition - (Defines::gridLength * 0.5), yPosition + (Defines::gridLength * 0.315)),
                          QSizeF(Defines::gridLength * 0.9, Defines::gridLength * 0.2));;
    }
    else
    {
        posText = QRectF(QPointF(xPosition - (Defines::gridLength * 0.3), yPosition - (Defines::gridLength * 0.35)),
                QSizeF(Defines::gridLength * 0.6, Defines::gridLength * 0.2));
        posValue = QRectF(QPointF(xPosition - (Defines::gridLength * 0.3), yPosition + (Defines::gridLength * 0.215)),
                        QSizeF(Defines::gridLength * 0.6, Defines::gridLength * 0.2));
    }

    paintInformation(painter, _name, static_cast<double>(_resistanceValue), posText, posValue, ComponentType::Resistor);
}
