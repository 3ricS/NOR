#ifndef NETWORKGRAPHICS_H
#define NETWORKGRAPHICS_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QMouseEvent>

#include <model/calculator.h>
#include <model/descriptionfield.h>
#include <model/resistor.h>
#include <model/powersupply.h>
#include <model/connection.h>
#include <model/filemanager.h>
#include <view/editview.h>

class FileManager;

class NetworkGraphics : public QGraphicsScene
{
public:
    NetworkGraphics();

    double calculate(void);
    void save(void);
    void load(void);
    void saveAs(void);

    void updateCalc(void);

    void mirrorComponent(Component* component);
    void turnComponentLeft(Component* componentToTurn);
    void turnComponentRight(Component* componentToTurn);
    void setOrientationOfComponent(Component* componentToTurn, Component::Orientation orientation);

    Component* createNewComponent( QPointF gridPosition, Component::ComponentType componentType, bool componentIsVertical);
    Component* duplicateComponent(Component* componentToDuplicate, int xPosition, int yPosition);
    Component* addResistor(QString name, int valueResistance, int _xPosition, int _yPosition, bool isVertical, int id = 0);
    Component* addPowerSupply(QString name, int x, int y, bool isVertical, int id = 0);
    DescriptionField* createDescriptionField(QPointF gridPosition);

    void addConnection(ComponentPort componentPortA, ComponentPort componentPortB);
    void deleteComponent(Component* component);
    void moveComponent(Component* componentToMove, QPointF gridPosition);

    //getter
    ComponentPort* getComponentPortAtPosition(QPointF scenePosition);
    Component* getComponentAtPosition(QPointF gridPosition);
    DescriptionField* getDescriptionAtPosition(QPointF gridPosition);
    bool isThereAComponentOrADescription(QPointF gridPosition);
    QList<Component*> getComponents(void) {return _componentList;}
    QList<Connection*> getConnections(void) {return _connectionList;}
    QList<DescriptionField*> getDescriptions(void) {return _descriptions;}
    QString getFileName(void);

private:
    static constexpr int _defaultSceneSize = 6000;

    int _resistorCount = 0;
    int _powerSupplyCount = 0;
    int _descriptionCount = 0;

    void addObject(Component* component);
    void connectComponentToNeighbours(Component* componentToConnectWithNeighbours);

    void reloadAll(void);

    QList<Component*> _componentList;
    QList<Connection*> _connectionList;
    QList<DescriptionField*> _descriptions;
    QGraphicsScene* _graphics = nullptr;
    FileManager* _manager = nullptr;
    Calculator _calculator = Calculator(_connectionList, _componentList);

    bool _isLoading = false;
};

#endif // NETWORKGRAPHICS_H
