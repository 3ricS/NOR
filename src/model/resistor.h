/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein Resistor ist eine Komponente der Schaltung
 *
 * Die Klasse Resistor ist ein Component, damit eine konkretere Beschreibung der Klasse Component.
 * Ein Resistor besitz einen Namen, einen Widerstandswert, X- und Y- Koordinaten, ein räumliche Ausrichtung und eine Id.
 */
#ifndef RESISTOR_H
#define RESISTOR_H

#include <model/component.h>

class Resistor : public Component
{
public:
    Resistor(QString name, long double valueResistance, int x, int y, bool isVertical, int id);

    long double getResistanceValue(void) {return _resistanceValue;}
    void setResistanceValue(long double resistance) { _resistanceValue = resistance;}


private:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual void setLabelPositions(QPainter* painter) override;

    long double _resistanceValue;
};

#endif // RESISTOR_H
