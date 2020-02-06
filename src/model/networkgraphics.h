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
#include <model/filemanager.h>
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

    void deleteItem(void);

    void calculate(void);
    void save(void);
    void load(void);

    //setter
    void setMode(MouseMode newMode) {_mouseMode = newMode;}

private:
    static constexpr int _defaultSceneSize = 6000;

    void addConnection(ComponentPort componentPortA, ComponentPort componentPortB);
    void addObject(Component* component);
    Component* getComponentAtPosition(QPointF gridPosition);
    bool isThereAComponent(QPointF position);
    ComponentPort* getComponentPortAtPosition(QPointF position);

    void pointToGrid(QPointF* position);
    void highlightRect(QPointF* position, QColor* highlightColor);
    void highlightSelectedRect(QPointF* position);

    void reloadAll(void);

    bool _mousIsPressed = false;
    bool _componentIsGrabbed = false;

    //TODO: gehört _connectionStarted & ConnectionStartPosition hierher?
    ComponentPort* _connectionStartComponentPort = new ComponentPort(nullptr, Component::Port::null);
    QGraphicsItem* _previousRect = nullptr;
    QGraphicsItem* _selectedItem = nullptr;
    QGraphicsItem* _selectedRect = nullptr;
    Component* _selectedComponentToMove;

    MouseMode _mouseMode = SelectionMode;
    QList<Component*> _componentList;
    QList<Connection*> _connectionList;
    QGraphicsScene* _graphics = nullptr;
};

#endif // NETWORKGRAPHICS_H
