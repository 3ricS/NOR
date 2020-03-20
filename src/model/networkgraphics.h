/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Eine PowerSupply ist eine Komponente der Schaltung
 *
 * Die Klasse NetworkGraphics ist die Modelklasse.
 *
 */
#ifndef NETWORKGRAPHICS_H
#define NETWORKGRAPHICS_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QUndoStack>

#include <model/description.h>
#include <model/resistor.h>
#include <model/powersupply.h>
#include <model/connection.h>
#include <model/filemanager.h>
#include <model/calculator.h>
#include <model/commands.h>

class FileManager;

class NetworkGraphics : public QGraphicsScene
{
    Q_OBJECT

public:
    NetworkGraphics();

    QPointF mapSceneToGrid(QPointF scenePosition);


    void save(void);
    void load(void);
    void saveAs(void);

    void updateCalc(void);

    //Selection
    void selectAll(void);
    void deselectAllItems(void);
    void selectObjectsInArea(QRectF selectionArea);

    void setOrientationOfComponent(Component* componentToTurn, Component::Orientation orientation);
    void turnSelectedComponentsRight(void);
    void turnComponentLeft(Component* componentToTurn);
    void turnComponentRight(Component* componentToTurn);


    //Copy-Paste
    void cutObjects(QList<GridObject*> objectsToCut);

    //Delete
    void deleteSelectedObjects(QList<GridObject*>& copiedObjects);

    //Add
    Component* addComponent(QPointF gridPosition, Component::ComponentType componentType, bool componentIsVertical);
    Component* addPowerSupply(QString name, int x, int y, bool isVertical, double voltage, int id = 0);
    Component* addResistor(QString name, long double valueResistance, int xPosition, int yPosition, bool isVertical, int id = 0);
    Description* addDescriptionField(QPointF gridPosition, bool isLoad, QString text = 0, int id = 0);
    void addConnection(ComponentPort componentPortA, ComponentPort componentPortB);

    //Move
    void moveMultiselectObjects(QList<GridObject*> selectedObjects, GridObject* objectToMove, QPointF scenePosition);

    //Edit
    void editComponent(Component* componentToEdit, QString newName, double newValue, Component::Orientation originalOrientation);
    void editDescription(Description* descriptionToEdit, QString newText);

    //Duplicate
    GridObject* duplicateGridObject(GridObject* gridObjectToDuplicate, int xPosition, int yPosition);
    void duplicateSelectedGridObjects();




    //Without Undo
    //only for QUndoComamnds
    Component* createNewComponentWithoutUndo(QPointF gridPosition,
                                             Component::ComponentType componentType, bool componentIsVertical);
    Component* addComponentWithoutUndo(Component* componentToAdd);
    Component* duplicateComponentWithoutUndo(Component* componentToDuplicate, int xPosition, int yPosition);
    void addConnectionWithoutUndo(Connection* connection);
    Connection* addConnectionWithoutUndo(ComponentPort componentPortA, ComponentPort componentPortB);
    Description* addDescriptionWithoutUndo(QPointF gridPosition, bool isLoad, QString text = 0, int id = 0);
    void addDescriptionWithoutUndo(Description* descriptionFieldToAdd);
    QList<Connection*> deleteComponentWithoutUndoAndGetDeletedConnections(Component* component);
    void deleteConnectionWithoutUndo(Connection* connection);
    void deleteDescriptionWithoutUndo(Description* description);
    void moveComponentWithoutUndo(GridObject* objectToMove, QPointF gridPosition);
    void editComponentWithoutUndo(Component* componentToEdit, QString newName, long double newValue);
    void turnComponentRightWithoutUndo(Component* componentToTurn);



    //getter
    QList<GridObject*> getObjects(void) {return _objects;}
    QList<Connection*> getConnections(void) {return _connections;}
    QList<Component*> getComponents(void);
    QList<Description*> getDescriptions(void);

    QList<GridObject*> getSelectedObjects(void);
    QList<Component*> getSelectedComponents(void);

    //Positions
    bool hasObjectAtPosition(QPointF scenePosition);
    GridObject* getObjectAtPosition(QPointF scenePosition);
    ComponentPort* getComponentPortAtPosition(QPointF scenePosition);
    Connection* getConnectionAtPosition(QPointF gridposition);

    Component* getComponentById(int id);

    //File
    bool hasChangedDocument(void) {return _hasChangedDocument;}
    QString getFileName(void);

    long double getResistanceValue(void) {return _resistanceValue;}
    QString getVoltageAndCurrentInformation(void);
    QUndoStack* getUndoStack(void) {return _undoStack;}

public slots:
    void hasChangedDocument(int idx);

signals:
    void resistanceValueChanged(void);
    void newNetworkIsLoad(void);
    void powerSupplyIsAllowed(bool isAllowed);
    void currentAndVoltageIsValid(bool isValid);


private:
    //Selection
    void selectAllGridObjects(void);
    void deselectAllConnections(void);
    void deselctAllGridObjects(void);

    //Edit
    void mirrorComponent(Component* component);

    //Move
    void moveObject(GridObject* objectToMove, QPointF scenePosition);
    void moveObjects(QList<GridObject*> objectsToMove, GridObject* objectToMove, int diffX, int diffY);


    Component* duplicateComponent(Component* componentToDuplicate, int xPosition, int yPosition);
    Description* duplicateDescription(Description* descriptionToDuplicate, int xPosition, int yPosition);
    bool lookingForFreeSpaceToDuplicate(int xPos, int yPos, int &xWaytoTheRight);

    void deleteSelectedGridObjects(QList<GridObject*>& copiedObjects);
    void deleteSelectedConnections();
    void deleteComponent(Component* componentToDelete);
    void deleteConnection(Connection* connectionToDelete);
    void deleteDescription(Description* descriptionFieldToDelete);


    //Getter
    QList<GridObject*> getGridObjectsInArea(QRectF selectionArea);
    QList<Connection*> getConnectionsInArea(QRectF selectionArea);

    QList<Connection*> getSelectedConnections();






    int _resistorCount = 0;
    int _powerSupplyCount = 0;
    int _descriptionCount = 0;

    void addObject(GridObject* component);

    QList<GridObject*>      _objects;

    QList<Connection*>       _connections;
    QGraphicsScene*          _graphics = nullptr;
    FileManager*             _manager = nullptr;
    QUndoStack*              _undoStack;

    bool _isLoading = false;
    bool _hasChangedDocument = false;
    long double _resistanceValue;
};

#endif // NETWORKGRAPHICS_H
