#include "resistor.h"


Resistor::Resistor(QString name, int valueResistance, int x, int y, bool isVertical)
        : Component(x, y, isVertical, name, valueResistance, Component::ComponentType::Resistor, 2)
{

    _resistorCount++;
}

Resistor::~Resistor()
{
    _resistorCount--;
}

void Resistor::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    //Zeichnen der Informationen (Name und Wert) in Abhängigkeit ob vertikal oder horizontal
    paintInformations(painter);

    if(_isVertical)
    {
        //Resistor has length of 120 and width of 60
        painter->drawRect(_xPosition - 20, _yPosition - 30, 40, 60);
        painter->drawLine(_xPosition, _yPosition - 30, _xPosition, _yPosition - 50);
        painter->drawLine(_xPosition, _yPosition + 30, _xPosition, _yPosition + 50);
    } else
    {
        painter->drawRect(_xPosition - 30, _yPosition - 20, 60, 40);
        painter->drawLine(_xPosition - 30, _yPosition + 0, _xPosition - 50, _yPosition + 0);
        painter->drawLine(_xPosition + 30, _yPosition + 0, _xPosition + 50, _yPosition + 0);
    }
}

void Resistor::paintInformations(QPainter* painter)
{
    //TODO: hier ist Dopplung des Codes; Switch-Case einfügen
    if(_isVertical)
    {
        // Zeichnen des Namens
        painter->drawText(_xPosition - 40, _yPosition - 40, _name);

        // Darstellung des Widerstandwertes
        if(_value < 1000)
        {
            painter->drawText(_xPosition - 60, _yPosition, QString::number(_value) + "Ω");
        } else
        {
            painter->drawText(_xPosition - 60, _yPosition, QString::number(_value / 1000) + "kΩ");
        }
    } else
    {
        // Zeichnen des Namens
        painter->drawText(_xPosition - 5, _yPosition - 30, _name);

        // Darstellung des Widerstandwertes
        if(_value < 1000)
        {
            painter->drawText(_xPosition - 5, _yPosition + 40, QString::number(_value) + "Ω");
        } else
        {
            painter->drawText(_xPosition - 5, _yPosition + 40, QString::number(_value / 1000) + "kΩ");
        }
    }
}
