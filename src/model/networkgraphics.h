#ifndef NETWORKGRAPHICS_H
#define NETWORKGRAPHICS_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include <model/resistor.h>
#include <model/powersupply.h>
#include <model/connection.h>

class NetworkGraphics : public QGraphicsScene
{
public:
    NetworkGraphics() : QGraphicsScene() {_graphics = new QGraphicsScene();}

    enum MouseMode{ResistorMode, PowerSupplyMode, ConnectionMode, DeleteMode, Mouse};
    void clickInterpretation(QPointF position);
    void setMode(MouseMode newMode) {_mouseMode = newMode;}

private:
    // TODO: Einbinden in NetworkView
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void addResistor(QString name, int value, int x, int y, bool isVertical);
    void addConnection(int xStart, int yStart, int xEnd, int yEnd);
    void addPowerSupply(QString name, int x, int y, bool isVertical);

    MouseMode _mouseMode = Mouse;
    QList<Component*> _componentList;
    QList<Connection*> _connectionList;
    QGraphicsScene* _graphics = nullptr;

};

#endif // NETWORKGRAPHICS_H
