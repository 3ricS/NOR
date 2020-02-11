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

    void calculate(void);
    void save(void);
    void load(void);
    void mirrorComponent(Component* component);

    Component* createNewComponent(QMouseEvent* mouseEvent, QPointF gridPosition,
                                  Component::ComponentType componentType, bool componentIsVertical);
    void addConnection(ComponentPort componentPortA, ComponentPort componentPortB);
    void deleteComponent(Component* component, QGraphicsItem* highlightedRect);

    //getter
    ComponentPort* getComponentPortAtPosition(QPointF scenePosition);
    Component* getComponentAtPosition(QPointF gridPosition);
    bool isThereAComponent(QPointF gridPosition);

private:
    static constexpr int _defaultSceneSize = 6000;

    void addObject(Component* component);

    void reloadAll(void);

    //TODO: gehört _connectionStarted & ConnectionStartPosition hierher?
    QGraphicsItem* _selectedRect = nullptr;
    Component* _selectedComponent = nullptr;
    //TODO: _selectedComponentToMove durch _selectedComponent ersetzen

    QList<Component*> _componentList;
    QList<Connection*> _connectionList;
    QGraphicsScene* _graphics = nullptr;
};

#endif // NETWORKGRAPHICS_H
