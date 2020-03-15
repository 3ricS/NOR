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

#include <model/descriptionfield.h>
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

    void save(void);
    void load(void);
    void saveAs(void);

    void updateCalc(void);

    void mirrorComponent(Component* component);
    void turnComponentLeft(Component* componentToTurn);
    void setOrientationOfComponent(Component* componentToTurn, Component::Orientation orientation);

    //with Undo
    void turnComponentRight(Component* componentToTurn);
    Component* addComponent(QPointF gridPosition, Component::ComponentType componentType, bool componentIsVertical);
    void addConnection(ComponentPort componentPortA, ComponentPort componentPortB);
    DescriptionField* addDescriptionField(QPointF gridPosition, bool isLoad, QString text = 0, int id = 0);
    void moveComponent(Component* componentToMove,DescriptionField* descriptionToMove ,QPointF gridPosition);
    void editComponent(Component* componentToEdit, QString newName, double newValue, Component::Orientation originalOrientation);
    void deleteComponent(Component* componentToDelete);
    void deleteConnection(Connection* connectionToDelete);
    void deleteDescription(DescriptionField* descriptionFieldToDelete);
    void editDescription(DescriptionField* descriptionToEdit, QString newText);
    void moveMultiselectComponents(QList<Component*> componentList, QList<DescriptionField*> descriptionList,
                                   Component* componentToMove, DescriptionField* descriptionToMove, int diffXAfterMoving, int diffYAfterMoving);


    Component* duplicateComponent(Component* componentToDuplicate, int xPosition, int yPosition);
    DescriptionField* duplicateDescription(DescriptionField* descriptionToDuplicate, int xPosition, int yPosition);


    Component* addResistor(QString name, double valueResistance, int xPosition, int yPosition, bool isVertical, int id = 0);
    Component* addPowerSupply(QString name, int x, int y, bool isVertical, double voltage, int id = 0);


    //only for QUndoCommands
    Component* createNewComponentWithoutUndo(QPointF gridPosition,
                                             Component::ComponentType componentType, bool componentIsVertical);
    void addComponentWithoutUndo(Component* componentToAdd);
    Component* duplicateComponentWithoutUndo(Component* componentToDuplicate, int xPosition, int yPosition);
    void addConnectionWithoutUndo(Connection* connection);
    Connection* addConnectionWithoutUndo(ComponentPort componentPortA, ComponentPort componentPortB);
    DescriptionField* addDescriptionFieldWithoutUndo(QPointF gridPosition, bool isLoad, QString text = 0, int id = 0);
    void addDescriptionFieldWithoutUndo(DescriptionField* descriptionFieldToAdd);
    QList<Connection*> deleteComponentWithoutUndoAndGetDeletedConnections(Component* component);
    void deleteConnectionWithoutUndo(Connection* connection);
    void deleteDescriptionWithoutUndo(DescriptionField* description);
    void moveComponentWithoutUndo(Component* componentToMove, DescriptionField* descriptionToMove , QPointF gridPosition);
    void editComponentWithoutUndo(Component* componentToEdit, QString newName, double newValue);
    void turnComponentRightWithoutUndo(Component* componentToTurn);

    //getter
    ComponentPort* getComponentPortAtPosition(QPointF scenePosition);
    Component* getComponentAtPosition(QPointF gridPosition);
    DescriptionField* getDescriptionAtPosition(QPointF gridPosition);
    bool isThereAComponentOrADescription(QPointF gridPosition);
    Connection* getConnectionAtPosition(QPointF gridposition);
    QList<Component*> getComponents(void) {return _componentList;}
    QList<Connection*> getConnections(void) {return _connectionList;}
    QList<DescriptionField*> getDescriptions(void) {return _descriptionList;}
    QString getFileName(void);
    double getResistanceValue(void) {return _resistanceValue;}
    bool isLoading(void) {return _isLoading;}
    QUndoStack* getUndoStack(void) {return _undoStack;}
    QString getVoltageAndCurrentInformation(void);
    QList<Component*> getSelectedComponents(void);
    QList<DescriptionField*> getSelectedDescriptionFields(void);

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

    void addObject(Component* component);
    void connectComponentToNeighbours(Component* componentToConnectWithNeighbours);

    QList<Component*>        _componentList;
    QList<Connection*>       _connectionList;
    QList<DescriptionField*> _descriptionList;
    QGraphicsScene*          _graphics = nullptr;
    FileManager*             _manager = nullptr;
    QUndoStack*              _undoStack;

    bool _isLoading = false;
    bool _hasChangedDocument = true;
    double _resistanceValue;
};

#endif // NETWORKGRAPHICS_H
