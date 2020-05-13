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

#include <model/calculator.h>
#include <model/commands.h>
#include <model/connection.h>
#include <model/description.h>
#include <model/filemanager.h>
#include <model/resistor.h>
#include <model/powersupply.h>

class FileManager;

class NetworkGraphics : public QGraphicsScene
{
    Q_OBJECT

public:
    NetworkGraphics();

    QPointF mapSceneToGrid(QPointF scenePosition);


    void save(void);
    void load(void);
    void loadFromFile(QString file);
    void saveAs(void);

    void updateCalc(void);

    //Selection
    void selectAll(void);
    void deselectAllItems(void);
    void selectObjectsInArea(QRectF selectionArea);

    void rotateSelectedComponentsRight(void);
    void rotateComponentLeft(Component* componentToRotate);
    void rotateComponentRight(Component* componentToRotate);


    //Copy-Paste
    void cutObjects(QList<GridObject*> objectsToCut);

    //Delete
    void deleteSelectedObjects(QList<GridObject*>& copiedObjects);

    //Add
    Component*      addComponent(QPointF scenePosition, Component::ComponentType componentType, bool componentIsVertical);
    Component*      addPowerSupply(QString name, int x, int y, bool isVertical, double voltage, int id = 0);
    Component*      addResistor(QString name, long double valueResistance, int xPosition, int yPosition, bool isVertical, int id = 0);
    Description*    addDescriptionField(QPointF scenePosition, bool isLoad, QString text = 0, int id = 0);
    void            addConnection(ComponentPort componentPortA, ComponentPort componentPortB);

    //Move
    void moveMultiselectObjects(QList<GridObject*> selectedObjects, GridObject* objectToMove, QPointF scenePosition);

    //Edit
    void editComponent(Component* componentToEdit, QString newName, double newValue, Component::Orientation originalOrientation);
    void editDescription(Description* descriptionToEdit, QString newText);

    //Duplicate
    GridObject* duplicateGridObject(GridObject* gridObjectToDuplicate, int xPosition, int yPosition);
    void        duplicateSelectedGridObjects(void);

    //Paste
    void pasteGridObjects(QList<GridObject*> objectsToPaste, QPointF positionToPaste);




    //Without Undo
    //only for QUndoComamnds
    Component*          createNewComponentWithoutUndo(QPointF gridPosition,
                                             Component::ComponentType componentType, bool componentIsVertical);
    Component*          addComponentWithoutUndo(Component* componentToAdd);
    Component*          duplicateComponentWithoutUndo(Component* componentToDuplicate, int xPosition, int yPosition);
    Connection*         addConnectionWithoutUndo(Connection* connection);
    Connection*         addConnectionWithoutUndo(ComponentPort componentPortA, ComponentPort componentPortB);
    Description*        addDescriptionWithoutUndo(QPointF gridPosition, bool isLoad, QString text = 0, int id = 0);
    void                addDescriptionWithoutUndo(Description* descriptionFieldToAdd);
    QList<Connection*>  deleteComponentWithoutUndoAndGetDeletedConnections(Component* componentToDelete);
    void                deleteConnectionWithoutUndo(Connection* connection);
    void                deleteDescriptionWithoutUndo(Description* description);
    void                moveComponentWithoutUndo(GridObject* objectToMove, QPointF gridPosition);
    void                editComponentWithoutUndo(Component* componentToEdit, QString newName, long double newValue);
    void                setOrientationOfComponentWithoutUndo(Component* componentToRotate, Component::Orientation orientation);
    void                rotateComponentRightWithoutUndo(Component* componentToRotate);
    void                rotateComponentLeftWithoutUndo(Component* componentToRotate);


    //Positions
    bool                hasObjectAtPosition(QPointF scenePosition);
    GridObject*         getObjectAtPosition(QPointF scenePosition);
    ComponentPort*      getComponentPortAtPosition(QPointF scenePosition);

    //getter
    QList<GridObject*>  getObjects(void) {return _objects;}
    QList<Connection*>  getConnections(void) {return _connections;}
    QList<Component*>   getComponents(void);
    QList<Description*> getDescriptions(void);

    QList<GridObject*>  getSelectedObjects(void);
    QList<Component*>   getSelectedComponents(void);


    Component*          getComponentById(int id);

    //File
    bool                hasChangedDocument(void) {return _hasChangedDocument;}
    QString             getFileName(void);

    long double         getResistanceValue(void) {return _resistanceValue;}
    QString             getVoltageAndCurrentInformation(void);
    QUndoStack*         getUndoStack(void) {return _undoStack;}

    //Calculator
    bool                hasUsedStarCalculation() {return _calculator->hasUsedStarCalculation();}

public slots:
    void hasChangedDocument(int idx);

signals:

    /*!
     * \brief Wird ausgesendet, sobald ein Widerstandswert geändert wird.
     */
    void resistanceValueChanged(void);

    /*!
     * \brief Wird ausgesendet, sobald ein Widerstandsnetzwerk geladen wurde.
     */
    void newNetworkIsLoad(void);

    /*!
     * \brief Wird ausgesendet, sobald eine PowerSupply erzeugt, gelöscht oder geladen wird.
     * \param isAllowed gibt an, ob eine Spannungsquelle auswählbar sein darf oder nicht.
     */
    void powerSupplyIsAllowed(bool isAllowed);

    /*!
     * \brief Wird ausgesendet, wenn der Widerstandswert gültig ist, also keine Stern-Dreieck-Umwandlung stattgefunden hat.
     * \param isValid gibt an, ob der Strom und Spannungswert interpretiert werden darf.
     */
    void currentAndVoltageIsValid(bool isValid);


private:
    //Selection
    void selectAllGridObjects(void);
    void deselectAllConnections(void);
    void deselctAllGridObjects(void);

    //Edit
    void rotateComponent(Component* componentToRotate, bool rotateClockwise);
    void mirrorComponent(Component* component);

    //Move
    void moveObject(GridObject* objectToMove, QPointF scenePosition);
    void moveObjects(QList<GridObject*> objectsToMove, GridObject* objectToMove, int diffX, int diffY);


    void            duplicateGridObjects(QList<GridObject*> objectsToDuplicate);
    Component*      duplicateComponent(Component* componentToDuplicate, int xPosition, int yPosition);
    Description*    duplicateDescription(Description* descriptionToDuplicate, int xPosition, int yPosition);

    bool lookingForFreeSpaceToDuplicate(int xPos, int yPos, int &xWaytoTheRight);
    void calculateDistanceToNextObject(int &i, GridObject* firstGridObject, int &xSpace, int &ySpace, QList<GridObject*> objects);

    void deleteSelectedGridObjects(QList<GridObject*>& copiedObjects);
    void deleteSelectedConnections();
    void deleteComponent(Component* componentToDelete);
    void deleteConnection(Connection* connectionToDelete);
    void deleteDescription(Description* descriptionFieldToDelete);


    //Getter
    QList<GridObject*> getGridObjectsInArea(QRectF selectionArea);
    QList<Connection*> getConnectionsInArea(QRectF selectionArea);

    QList<Connection*> getSelectedConnections();

    void addObject(GridObject* component);

    void updateNewDocument();


    int _resistorCount = 0;
    int _powerSupplyCount = 0;
    int _descriptionCount = 0;


    QList<GridObject*>      _objects;

    QList<Connection*>       _connections;
    QGraphicsScene*          _graphics = nullptr;
    FileManager*             _manager = nullptr;
    Calculator*              _calculator = nullptr;
    QUndoStack*              _undoStack;

    bool _isLoading = false;
    bool _hasChangedDocument = false;
    long double _resistanceValue;
};

#endif // NETWORKGRAPHICS_H
