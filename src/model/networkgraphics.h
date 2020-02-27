#ifndef NETWORKGRAPHICS_H
#define NETWORKGRAPHICS_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QUndoStack>

#include <model/descriptionfield.h>
#include <model/resistor.h>
#include <model/powersupply.h>
#include <model/connection.h>
#include <model/filemanager.h>
#include <model/puzzlecalculator.h>
#include <model/commands.h>

class FileManager;

class NetworkGraphics : public QGraphicsScene
{
    Q_OBJECT

public:
    NetworkGraphics();

    void save(void);
    void load(void);
    void saveAs(void);

    void updateCalc(void);

    void mirrorComponent(Component* component);
    void turnComponentLeft(Component* componentToTurn);
    void turnComponentRight(Component* componentToTurn);
    void setOrientationOfComponent(Component* componentToTurn, Component::Orientation orientation);

    //with Undo
    Component* createNewComponent( QPointF gridPosition, Component::ComponentType componentType, bool componentIsVertical);
    void addConnection(ComponentPort componentPortA, ComponentPort componentPortB);
    void moveComponent(Component* componentToMove,DescriptionField* descriptionToMove ,QPointF gridPosition);
    void editComponent(Component* componentToEdit, QString newName, double newValue, Component::Orientation originalOrientation);

    Component* duplicateComponent(Component* componentToDuplicate, int xPosition, int yPosition);
    DescriptionField* duplicateDescription(DescriptionField* descriptionToDuplicate, int xPosition, int yPosition);
    Component* addResistor(QString name, int valueResistance, int _xPosition, int _yPosition, bool isVertical, int id = 0);
    Component* addPowerSupply(QString name, int x, int y, bool isVertical, int id = 0);
    DescriptionField* createDescriptionField(QPointF gridPosition, bool isLoad, QString text = 0, int id = 0);



    void deleteDescription(DescriptionField* description);



    //for actions
    Component* createNewComponentWithoutUndo(QPointF gridPosition,
                                             Component::ComponentType componentType, bool componentIsVertical);
    void deleteComponentWithoutUndo(Component* component);
    Connection* addConnectionWithoutUndo(ComponentPort componentPortA, ComponentPort componentPortB);
    void deleteConnectionWithoutUndo(Connection* connection);
    void moveComponentWithoutUndo(Component* componentToMove, DescriptionField* descriptionToMove , QPointF gridPosition);
    void editComponentWithoutUndo(Component* componentToEdit, QString newName, double newValue);


    //getter
    ComponentPort* getComponentPortAtPosition(QPointF scenePosition);
    Component* getComponentAtPosition(QPointF gridPosition);
    DescriptionField* getDescriptionAtPosition(QPointF gridPosition);
    bool isThereAComponentOrADescription(QPointF gridPosition);
    Connection* getConnectionAtPosition(QPointF gridposition);
    QList<Component*> getComponents(void) {return _componentList;}
    QList<Connection*> getConnections(void) {return _connectionList;}
    QList<DescriptionField*> getDescriptions(void) {return _descriptions;}
    QString getFileName(void);
    double getResistanceValue(void) {return _resistanceValue;}
    bool isLoading(void) {return _isLoading;}
    QUndoStack* getUndoStack(void) {return _undoStack;}

signals:
    void resistanceValueChanged(void);
    void newNetworkIsLoad();

private:
    static constexpr int _defaultSceneSize = 6000;

    int _resistorCount = 0;
    int _powerSupplyCount = 0;
    int _descriptionCount = 0;

    void addObject(Component* component);
    void connectComponentToNeighbours(Component* componentToConnectWithNeighbours);

    QList<Component*>        _componentList;
    QList<Connection*>       _connectionList;
    QList<DescriptionField*> _descriptions;
    QGraphicsScene*          _graphics = nullptr;
    FileManager*             _manager = nullptr;
    PuzzleCalculator         _puzzleCalculator = PuzzleCalculator();
    QUndoStack*              _undoStack;

    bool _isLoading = false;
    double _resistanceValue;
};

#endif // NETWORKGRAPHICS_H
