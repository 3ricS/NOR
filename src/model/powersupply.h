/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Eine PowerSupply ist eine Komponente der Schaltung
 *
 * Die Klasse PowerSupply ist ein Component, damit eine konkretere Beschreibung der Klasse Component
 * Eine PowerSupply besitz einen Namen, X- und Y- Koordinaten, ein räumliche Ausrichtung und eine Id.
 */
#ifndef POWERSUPPLY_H
#define POWERSUPPLY_H

#include <model/component.h>

class PowerSupply : public Component
{
public:
    PowerSupply(QString name, int x, int y, bool isVertical, int id);

    //Methoden
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


    //getter

    //setter

    //Klassenmethoden

private:

    //Klassenvariablen

    //Methodenn
    virtual void paintInformations(QPainter* painter) override;

    //Variablen
};

#endif // POWERSUPPLY_H
