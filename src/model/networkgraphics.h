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
    void selectAllGridObjects(void);
    void deselectAllItems(void);
    void deselectAllConnections(void);
    void deselctAllGridObjects(void);
    void selectObjectsAtPosition(QPointF scenePosition);
    void selectObjectsInArea(QRectF selectionArea);
    QList<Component*> findSelectedComponents(void);
    QList<Description*> findSelectedDescription(void);
    void turnSelectedComponentsRight(void);

    void mirrorComponent(Component* component);
    void turnComponentLeft(Component* componentToTurn);
    void setOrientationOfComponent(Component* componentToTurn, Component::Orientation orientation);

    //Copy-Paste
    void cutComponent(Component* componentToCut);
    void cutComponent(QList<Component*> components);
    void cutDescription(Description* descriptionToCut);
    void cutDescription(QList<Description*> descriptions);

    //with Undo
    void turnComponentRight(Component* componentToTurn);
    Component* addComponent(QPointF gridPosition, Component::ComponentType componentType, bool componentIsVertical);
    void addConnection(ComponentPort componentPortA, ComponentPort componentPortB);
    Description* addDescriptionField(QPointF gridPosition, bool isLoad, QString text = 0, int id = 0);
    void moveObject(GridObject* objectToMove, QPointF gridPosition);
    void editComponent(Component* componentToEdit, QString newName, double newValue, Component::Orientation originalOrientation);
    void deleteComponent(Component* componentToDelete);
    void deleteConnection(Connection* connectionToDelete);
    void deleteDescription(Description* descriptionFieldToDelete);
    void editDescription(Description* descriptionToEdit, QString newText);
    void moveMultiselectComponents(QList<GridObject*> objects, GridObject* objectToMove,
                                   int diffXAfterMoving, int diffYAfterMoving);



    Component* duplicateComponent(Component* componentToDuplicate, int xPosition, int yPosition);
    Description* duplicateDescription(Description* descriptionToDuplicate, int xPosition, int yPosition);


    Component* addResistor(QString name, long double valueResistance, int xPosition, int yPosition, bool isVertical, int id = 0);
    Component* addPowerSupply(QString name, int x, int y, bool isVertical, double voltage, int id = 0);


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

    QList<Component*> getComponents(void);
    QList<Description*> getDescriptions(void);
    QList<GridObject*> getObjects(void) {return _objects;}
    QList<Connection*> getConnections(void) {return _connections;}

    GridObject* getObjectAtPosition(QPointF scenePosition);
    ComponentPort* getComponentPortAtPosition(QPointF scenePosition);
    bool hasObjectAtPosition(QPointF scenePosition);
    Connection* getConnectionAtPosition(QPointF gridposition);

    QString getFileName(void);
    long double getResistanceValue(void) {return _resistanceValue;}
    QUndoStack* getUndoStack(void) {return _undoStack;}
    QString getVoltageAndCurrentInformation(void);
    QList<GridObject*> getSelectedObjects(void);

    Component* getComponentById(int id);

    bool hasChangedDocument(void) {return _hasChangedDocument;}

public slots:
    void hasChangedDocument(int idx);

signals:
    void resistanceValueChanged(void);
    void newNetworkIsLoad(void);
    void powerSupplyIsAllowed(bool isAllowed);
    void currentAndVoltageIsValid(bool isValid);


private:
    QList<GridObject*> getGridObjectsInArea(QRectF selectionArea);
    QList<Connection*> getConnectionsInArea(QRectF selectionArea);

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
