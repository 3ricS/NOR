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
    QRectF boundingRect(void) const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    //Methoden
    Component* getComponentA(void){return _componentA;}
    Component* getComponentB(void){return _componentB;}
    Component::Port getPortA(void){return _portA;}
    Component::Port getPortB(void){return _portB;}

private:
    Component* _componentA;
    Component::Port _portA;
    Component* _componentB;
    Component::Port _portB;
};

#endif // CONNECTION_H
