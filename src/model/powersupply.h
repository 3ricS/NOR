/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Eine PowerSupply ist eine Komponente der Schaltung
 *
 * Die Klasse PowerSupply ist ein Component, damit eine konkretere Beschreibung der Klasse Component
 * Eine PowerSupply besitz einen Namen, X- und Y- Koordinaten, ein räumliche Ausrichtung, einen Spannungswert und eine Id.
 */
#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

#include <model/component.h>


class PowerSupply : public Component
{
public:
    PowerSupply(QString name, int x, int y, bool isVertical, double voltage, int id);

    //Methoden
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:

    //Methodenn
    virtual void setLabelPositions(QPainter* painter) override;
};

#endif // POWERSUPPLY_H
