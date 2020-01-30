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
    NetworkGraphics();

    enum MouseMode{ResistorMode, PowerSupplyMode, ConnectionMode, Mouse};
    void mouseReleaseInterpretation(QPointF position);
    void mousePressInterpretation(QPointF position);
    void setMode(MouseMode newMode) {_mouseMode = newMode;}

private:
    static constexpr int _defaultSceneSize = 6000;

    void addConnection(int xStart, int yStart, int xEnd, int yEnd);
    void addObject(Component* component);

    void pointToGrid(QPointF* position);

    //TODO: gehört _connectionStarted & ConnectionStartPosition hierher?
    bool _connectionStarted = false;
    QPointF _connectionPointStart;

    MouseMode _mouseMode = Mouse;
    QList<Component*> _componentList;
    QList<Connection*> _connectionList;
    QGraphicsScene* _graphics = nullptr;
};

#endif // NETWORKGRAPHICS_H
