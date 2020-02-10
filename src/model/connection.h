#ifndef CONNECTION_H
#define CONNECTION_H

#include <model/componentport.h>

#include <QPainter>
#include <algorithm>

class Connection : public QGraphicsItem
{
public:
    Connection(ComponentPort componentPortA, ComponentPort componentPortB);

    //TODO: boundingRect muss noch beschrieben werden
    QRectF boundingRect(void) const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void changePortOfComponentPortWithComponent(Component* componentOfComponentPortToChangePortOf);

    //Methoden
    ComponentPort getComponentPortA(void) const {return _componentPortA;}
    ComponentPort getComponentPortB(void) const {return _componentPortB;}
    bool hasComponent(Component* searchedComponent);

private:
    ComponentPort _componentPortA;
    ComponentPort _componentPortB;
};

#endif // CONNECTION_H
