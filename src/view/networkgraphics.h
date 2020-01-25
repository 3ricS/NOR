#ifndef NETWORKGRAPHICS_H
#define NETWORKGRAPHICS_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include <model/model.h>

class NetworkGraphics : public QGraphicsScene
{
public:

    NetworkGraphics(Model* model) : QGraphicsScene(), _model(model) {_graphics = new QGraphicsScene();}
    void addResistor(int x, int y);
    void addConnection(int x_start, int y_start, int x_end, int y_end);
    void addPowerSupply(int x, int y);


    //Only for testing in public; see mainwindow.cpp connect()
    QGraphicsScene* _graphics = nullptr;

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    double _zoomFactor = 1.0;
    Model* _model;

};

#endif // NETWORKGRAPHICS_H
