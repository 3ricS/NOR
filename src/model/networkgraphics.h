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
    void paintResistor(int x, int y, bool isVertical);
    void paintResistor90Degree(int x, int y);
    void paintConnection(int x_start, int y_start, int x_end, int y_end);
    void paintPowerSupply(int x, int y, bool isVertical);
    void paintPowerSupply90Degree(int x, int y);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    QGraphicsScene* _graphics = nullptr;

    double _zoomFactor = 1.0;
    Model* _model;
    QPointF _position;

};

#endif // NETWORKGRAPHICS_H