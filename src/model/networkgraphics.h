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
    void deselectAllItems(void);
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


    //only for QUndoCommands
    void cutComponentWithoutUndo(Component* componentToCut);
    void cutDescriptionWithoutUndo(Description* descriptionToCut);
    Component* createNewComponentWithoutUndo(QPointF gridPosition,
                                             Component::ComponentType componentType, bool componentIsVertical);
    void addComponentWithoutUndo(Component* componentToAdd);
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
    ComponentPort* getComponentPortAtPosition(QPointF scenePosition);

    GridObject* getObjectAtPosition(QPointF scenePosition);
    bool hasObjectAtPosition(QPointF scenePosition);
    Connection* getConnectionAtPosition(QPointF gridposition);
    QList<Component*> getComponents(void) {return _components;}
    QList<Description*> getDescriptions(void) {return _descriptions;}
    QList<GridObject*> getObjects(void) {return _objects;}
    QList<Connection*> getConnections(void) {return _connections;}
    QString getFileName(void);
    long double getResistanceValue(void) {return _resistanceValue;}
    bool isLoading(void) {return _isLoading;}
    QUndoStack* getUndoStack(void) {return _undoStack;}
    QString getVoltageAndCurrentInformation(void);
    QList<GridObject*> getSelectedObjects(void);
    QList<Description*> getSelectedDescriptionFields(void);

    bool hasChangedDocument(void);

public slots:
    void hasChangedDocument(int idx);

signals:
    void resistanceValueChanged(void);
    void newNetworkIsLoad(void);
    void powerSupplyIsAllowed(bool isAllowed);
    void currentAndVoltageIsValid(bool isValid);


private:
    static constexpr int _defaultSceneSize = 6000;

    int _resistorCount = 0;
    int _powerSupplyCount = 0;
    int _descriptionCount = 0;

    void addObject(GridObject* component);

    QList<Component*>        _components;
    QList<Description*> _descriptions;
    QList<GridObject*>      _objects;

    QList<Connection*>       _connections;
    QGraphicsScene*          _graphics = nullptr;
    FileManager*             _manager = nullptr;
    QUndoStack*              _undoStack;

    bool _isLoading = false;
    bool _hasChangedDocument = true;
    long double _resistanceValue;
};

#endif // NETWORKGRAPHICS_H
