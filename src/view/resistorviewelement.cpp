#include "resistorviewelement.h"


void ResistorViewElement::show(QGraphicsScene* scene, int x, int y)
{
    //Resistor has length of 120 and width of 60
    scene->addRect(x, y + 20, 60, 80);
    scene->addLine(x + 30, y + 0, x + 30, y + 20);
    scene->addLine(x + 30, y + 100, x + 30, y + 120);
}

void ResistorViewElement::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug("Widerstand gedrückt");
}

QRectF ResistorViewElement::boundingRect() const
{

}

void ResistorViewElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}
