#include "defines.h"
#include "powersupply.h"
#include "connection.h"

#include <QLocale>

PowerSupply::PowerSupply(QString name, int x, int y, bool isVertical, double voltage, int id, NetworkGraphics* model) :
        Component(QPointF(x, y), isVertical, name, voltage, Component::ComponentType::PowerSupply, id, model)
{
}

void PowerSupply::paint(QPainter* painter, [[maybe_unused]] const QStyleOptionGraphicsItem* option,
                        [[maybe_unused]] QWidget* widget)
{
    int xPosition = _position.x();
    int yPosition = _position.y();

    int deviationPortA = 0;
    int deviationPortB = 0;
    calculateDeviationPortAB(deviationPortA, deviationPortB);

    painter->drawEllipse(xPosition - (Defines::gridLength * Defines::gridObjectWidth / 2), yPosition - (Defines::gridLength * Defines::gridObjectWidth / 2),
                         Defines::gridLength * Defines::gridObjectWidth, Defines::gridLength * Defines::gridObjectWidth);
    if(isVertical())
    {
        painter->drawLine(xPosition, yPosition + (Defines::gridLength / 2) - Connection::_circleRadius - deviationPortB,
                xPosition, yPosition - (Defines::gridLength / 2) + Connection::_circleRadius + deviationPortA);
    }
    else
    {
        painter->drawLine(xPosition - (Defines::gridLength / 2) + Connection::_circleRadius + deviationPortA, yPosition,
                          xPosition + (Defines::gridLength / 2) - Connection::_circleRadius - deviationPortB, yPosition);
    }
    if(_isSelected)
    {
        paintSelectionHighlight(painter);
    }

    setLabelPositions(painter);
}

void PowerSupply::setLabelPositions(QPainter* painter)
{
    QFont q;
    q.setPixelSize(13);
    painter->setFont(q);


    int xPosition = _position.x();
    int yPosition = _position.y();
    QRectF posName(QPointF(xPosition - (Defines::gridLength * 0.3), yPosition - (Defines::gridLength * 0.43)),
                   QSize(Defines::gridLength * 0.7, 20));
    painter->drawText(posName, Qt::AlignLeft, _name);
    double voltageWithoutUnit;
    QString unitString = "";
    // Darstellung des Spannungswertes
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
    QString displayString = valueString + " " + unitString + "V";
    QRectF posValue;
    if(isVertical())
    {
        posValue = QRectF(QPointF(xPosition - (Defines::gridLength * 0.4), yPosition + (Defines::gridLength * 0.32)),
                          QSize(Defines::gridLength * 0.8, 20));
    }
    else
    {
        posValue = QRectF(QPointF(xPosition - (Defines::gridLength * 0.4), yPosition + (Defines::gridLength * 0.32)),
                          QSize(Defines::gridLength * 0.7, 20));
    }


    paintInformation(painter, _name, _voltage, posName, posValue, ComponentType::PowerSupply);
}
