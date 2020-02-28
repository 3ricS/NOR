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
    Component* addComponent(QPointF gridPosition, Component::ComponentType componentType, bool componentIsVertical);
    void addConnection(ComponentPort componentPortA, ComponentPort componentPortB);
    DescriptionField* addDescriptionField(QPointF gridPosition, bool isLoad, QString text = 0, int id = 0);
    void moveComponent(Component* componentToMove,DescriptionField* descriptionToMove ,QPointF gridPosition);
    void editComponent(Component* componentToEdit, QString newName, double newValue, Component::Orientation originalOrientation);
    void deleteComponent(Component* componentToDelete);
    void deleteConnection(Connection* connectionToDelete);
    void deleteDescription(DescriptionField* descriptionFieldToDelete);


    Component* duplicateComponent(Component* componentToDuplicate, int xPosition, int yPosition);
    DescriptionField* duplicateDescription(DescriptionField* descriptionToDuplicate, int xPosition, int yPosition);


    Component* addResistor(QString name, int valueResistance, int _xPosition, int _yPosition, bool isVertical, int id = 0);
    Component* addPowerSupply(QString name, int x, int y, bool isVertical, int id = 0);





    //only for QUndoCommands
    Component* createNewComponentWithoutUndo(QPointF gridPosition,
                                             Component::ComponentType componentType, bool componentIsVertical);
    void addComponentWithoutUndo(Component* componentToAdd);
    void addConnectionWithoutUndo(Connection* connection);
    Connection* addConnectionWithoutUndo(ComponentPort componentPortA, ComponentPort componentPortB);
    DescriptionField* addDescriptionFieldWithoutUndo(QPointF gridPosition, bool isLoad, QString text = 0, int id = 0);
    void addDescriptionFieldWithoutUndo(DescriptionField* descriptionFieldToAdd);
    QList<Connection*> deleteComponentWithoutUndoAndGetDeletedConnections(Component* component);
    void deleteConnectionWithoutUndo(Connection* connection);
    void deleteDescriptionWithoutUndo(DescriptionField* description);
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
