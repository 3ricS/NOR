#ifndef NETWORKGRAPHICS_H
#define NETWORKGRAPHICS_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QMessageBox>

#include <model/calculator.h>
#include <model/resistor.h>
#include <model/powersupply.h>
#include <model/connection.h>
#include <view/editview.h>

class NetworkGraphics : public QGraphicsScene
{
public:
    NetworkGraphics();

    enum MouseMode{ResistorMode, PowerSupplyMode, ConnectionMode, SelectionMode};
    void mouseReleaseInterpretation(QPointF position);
    void mousePressInterpretation(QPointF position);
    void mouseDoublePressInterpretation(QPointF position);
    void mouseMoveInterpretation(QPointF position);

    void calculate(void);

    //setter
    void setMode(MouseMode newMode) {_mouseMode = newMode;}

private:
    static constexpr int _defaultSceneSize = 6000;

    void addConnection(Component* componentA, Component::Port componentAPort, Component* componentB, Component::Port componentBPort);
    void addObject(Component* component);
    Component* getComponentAtPosition(QPointF gridPosition);
    bool isThereAComponent(QPointF position);
    Component* getComponentWithPortAtPosition(QPointF position, bool& hasFoundPort);

    void pointToGrid(QPointF* position);
    void highlightResistorEnd(QPointF* position);

    //TODO: gehört _connectionStarted & ConnectionStartPosition hierher?
    bool _connectionStarted = false;
    Component* _connectionComponentStart;
    Component::Port _connectionComponentStartPort;
    QGraphicsItem* _previousRect = nullptr;
    Component* _selectedComponentToMove;

    MouseMode _mouseMode = SelectionMode;
    QList<Component*> _componentList;
    QList<Connection*> _connectionList;
    QGraphicsScene* _graphics = nullptr;
    EditView* _editingView = nullptr;
};

#endif // NETWORKGRAPHICS_H
