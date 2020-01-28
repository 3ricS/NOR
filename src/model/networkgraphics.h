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
    void paintResistor(int x, int y);
    void paintResistor90Degree(int x, int y);
    void paintConnection(int x_start, int y_start, int x_end, int y_end);
    void paintPowerSupply(int x, int y);
    void paintPowerSupply90Degree(int x, int y);


    //Only for testing in public; see mainwindow.cpp connect()
    QGraphicsScene* _graphics = nullptr;

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    double _zoomFactor = 1.0;
    Model* _model;
    QPointF _position;
    QPointF _positionEnd;

};

#endif // NETWORKGRAPHICS_H