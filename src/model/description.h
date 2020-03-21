/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein Description ist ein Textfeld um Notizen und Information zum Netzwerk an die Schaltung zu schreiben.
 *
 * Ein Textfeld besitz die Größe einse Gitters, in dem ein ausgewählter Text stehen kann.
 * Ein Description ist ein QGraphicsItem, damit ein Element der QGraphicsScene und kann sich selbst in die QGraphicsScene
 * zeichnen.
 */
#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <QGraphicsItem>
#include <QPainter>
#include "gridobject.h"

class Description : public GridObject
{
public:
    Description(int x, int y, int id, QString text = 0);

    //Getter
    QString getText(void) const {return _text;}

    //Setter
    void setText(const QString text) {_text = text;}

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QString _text = "Hier könnte dein Text stehen!";
};

#endif // DESCRIPTIONFIELD_H
