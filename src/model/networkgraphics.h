#ifndef NETWORKGRAPHICS_H
#define NETWORKGRAPHICS_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QMouseEvent>

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

    double calculate(void);
    void save(void);
    void load(void);
    void saveAs(void);
    void mirrorComponent(Component* component);

    Component* createNewComponent(QMouseEvent* mouseEvent, QPointF gridPosition,
                                  Component::ComponentType componentType, bool componentIsVertical);
    Component* addResistor(int valueResistance, int _xPosition, int _yPosition, bool isVertical);
    Component* addPowerSupply(int x, int y, bool isVertical);
    void addConnection(ComponentPort componentPortA, ComponentPort componentPortB);
    void deleteComponent(Component* component);
    void moveComponent(Component* componentToMove, QPointF gridPosition);

    //getter
    ComponentPort* getComponentPortAtPosition(QPointF scenePosition);
    Component* getComponentAtPosition(QPointF gridPosition);
    bool isThereAComponent(QPointF gridPosition);

private:
    static constexpr int _defaultSceneSize = 6000;

    int _resistorCount = 0;
    int _powerSupplyCount = 0;

    void addObject(Component* component);
    void connectComponentToNeighbours(Component* componentToConnectWithNeighbours);

    void reloadAll(void);

    QList<Component*> _componentList;
    QList<Connection*> _connectionList;
    QGraphicsScene* _graphics = nullptr;
    FileManager* _manager = nullptr;
};

#endif // NETWORKGRAPHICS_H
