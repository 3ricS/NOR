#ifndef CONNECTION_H
#define CONNECTION_H

#include <model/component.h>

#include <QPainter>
#include <algorithm>

class Connection : public QGraphicsItem
{
public:
    Connection(Component* componentA, Component::Port componentAPort, Component* componentB, Component::Port componentBPort);

    //TODO: boundingRect muss noch beschrieben werden
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    //Methoden

private:
    Component* _componentA;
    Component::Port _portA;
    Component* _componentB;
    Component::Port _portB;
};

#endif // CONNECTION_H
