#ifndef CONNECTION_H
#define CONNECTION_H

#include <model/component.h>

#include <QPainter>

class Connection : QGraphicsItem
{
public:
    Connection(int xStart, int yStart, int xEnd, int yEnd);

    //Methoden
    void show(QGraphicsScene* scene);

    // Component interface
    int getXStartPosition() const {return _xStart;}
    int getYStartPosition() const {return _yStart;}
    int getXEndPosition() const {return  _xEnd;}
    int getYEndPosition() const {return _yEnd;}

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    //TODO: boundingRect muss noch beschrieben werden
    QRectF boundingRect() const;
    int _xStart;
    int _yStart;
    int _xEnd;
    int _yEnd;

};

#endif // CONNECTION_H
