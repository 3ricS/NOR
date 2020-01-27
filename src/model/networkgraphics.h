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


private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    QGraphicsScene* _graphics = nullptr;
    Model* _model;

};

#endif // NETWORKGRAPHICS_H
