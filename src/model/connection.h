#ifndef CONNECTION_H
#define CONNECTION_H

#include <model/component.h>

#include <QPainter>
#include <algorithm>

class Connection : public QGraphicsItem
{
public:
    Connection(int xStart, int yStart, int xEnd, int yEnd);

    //TODO: boundingRect muss noch beschrieben werden
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    //Methoden
    void show(QGraphicsScene* scene);

    // Component interface
    int getXStartPosition(void) const {return _xStart;}
    int getYStartPosition(void) const {return _yStart;}
    int getXEndPosition(void) const {return  _xEnd;}
    int getYEndPosition(void) const {return _yEnd;}

private:
    int _xStart;
    int _yStart;
    int _xEnd;
    int _yEnd;

};

#endif // CONNECTION_H
