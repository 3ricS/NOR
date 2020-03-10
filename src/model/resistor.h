/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein Resistor ist eine Komponente der Schaltung
 *
 * Die Klasse Resistor ist ein Component, damit eine konkretere Beschreibung der Klasse Component
 * Ein Resistor besitz einen Namen, einen Widerstandswert, X- und Y- Koordinaten, ein räumliche Ausrichtung und eine Id.
 */
#ifndef RESISTOR_H
#define RESISTOR_H

#include <model/component.h>

class Resistor : public Component
{
public:
    Resistor(QString _name, int valueResistance, int _xPosition, int _yPosition, bool isVertical, int id);

    //Methoden
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:

    //Methoden
    virtual void paintInformations(QPainter* painter) override;
    void paintOrientationSensitiv(QPainter* painter, int xPosText, int yPosText, int xPosValue, int yPosValue);
};

#endif // RESISTOR_H
