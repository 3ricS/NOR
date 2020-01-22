#ifndef NETWORKGRAPHICS_H
#define NETWORKGRAPHICS_H

#include <QGraphicsScene>
#include <string>

#include "view/resistorviewelement.h"

class NetworkGraphics : public QGraphicsScene
{
public:
    NetworkGraphics() : QGraphicsScene() {_graphics = new QGraphicsScene();}
    void addResistor(int value, int x, int y);
    void addConnection(int x_start, int y_start, int x_end, int y_end);

private:
    QGraphicsScene* _graphics = nullptr;
};

#endif // NETWORKGRAPHICS_H
