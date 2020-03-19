#include "defines.h"
#include "model/networkgraphics.h"
#include "model/calculator.h"

#include <QDebug>

NetworkGraphics::NetworkGraphics() : QGraphicsScene(), _graphics(new QGraphicsScene()), _undoStack(new QUndoStack(this))
{
    // die Anfangsgröße wird initialisiert
    setSceneRect(-_defaultSceneSize, -_defaultSceneSize, _defaultSceneSize, _defaultSceneSize);
    _manager = new FileManager(this);
    connect(_undoStack, SIGNAL(indexChanged(int)), this, SLOT(hasChangedDocument(int)));
}

/*!
 * \brief Fügt eine Verbindung dem Netzwerk hinzu.
 *
 * \param   componentPortA  Port der Komponente 1.
 * \param   componentPortB  Port der Komponente 2.
 *
 * Zuerst wird geprüft, ob die Verbindung bereits der connectionList hinzugefügt wurde.
 * Ist noch keine Verbindung vorhanden, wird diese hinzugefügt.
 */
Connection* NetworkGraphics::addConnectionWithoutUndo(ComponentPort componentPortA, ComponentPort componentPortB)
{
    Connection* connection = new Connection(componentPortA, componentPortB, this);
    Connection* createdConnection = nullptr;
    bool isAlreadyExisting = false;
    for (Connection* otherConnection : _connections)
    {
        if (*otherConnection == *connection)
        {
            isAlreadyExisting = true;
            createdConnection = otherConnection;
            break;
        }
    }
    if (!isAlreadyExisting)
    {
        _connections.append(connection);
        createdConnection = connection;
        addItem(connection);
        update();
    }
    else
    {
        delete connection;
    }

    if (!_isLoading)
    {
        updateCalc();
    }

    return createdConnection;
}


void NetworkGraphics::addObject(GridObject* gridObject)
{
    _objects.append(gridObject);
    //Hier kann der Widerstand direkt mit den Nachbarn bzw. angrenzenden Widerständen verbunden werden
    //connectComponentToNeighbours(component);
    addItem(gridObject);

    updateCalc();
}

/*!
 * \brief Gibt eine Describtion aus der Liste aus Descriptions aus, welches an den Soll-Koordinaten x und y ist.
 *
 * \param   scenePosition   ist die zu überprüfende Gitterposition
 * \return
 *
 * Es werden nacheinander Description aus der Liste genommen und verglichen, ob ihre Koordinaten mit den Soll-Koordinaten übereinstimmen.
 * Wenn ein Component mit den Soll-Koordinaten gefunden wurde, wird dieses zurückgegeben, ansonsten wird der Nullpointer zurückgegeben.
 */
GridObject* NetworkGraphics::getObjectAtPosition(QPointF scenePosition)
{
    QPointF gridPosition = mapSceneToGrid(scenePosition);
    for (GridObject* gridObject : _objects)
    {
        bool equalPosition = gridObject->getPosition() == gridPosition;
        if (equalPosition)
        {
            return gridObject;
        }
    }
    return nullptr;
}

/*!
 * \brief Prüft ob sich an einer bestimmten Koordinate, eine Komponente oder Textfeld befindet.
 *
 * \param   scenePosition    ist die zu überprüfende GitterPositon
 * \return Gibt zurück, ob sich an der Gitterposition eine Komponente oder Textfeld befindet.
 *
 */
bool NetworkGraphics::hasObjectAtPosition(QPointF scenePosition)
{
    return getObjectAtPosition(scenePosition) != nullptr;
}

Connection* NetworkGraphics::getConnectionAtPosition(QPointF gridposition)
{
    for (Connection* connection : _connections)
    {
        for (QRect* hitBox : connection->getHitboxList())
        {
            bool equalX = ((gridposition.toPoint().x() >= hitBox->x()) &&
                           (gridposition.toPoint().x() <= (hitBox->x() + hitBox->width())));
            bool equalY = ((gridposition.toPoint().y() >= hitBox->y()) &&
                           (gridposition.toPoint().y() <= (hitBox->y() + hitBox->height())));
            if (equalX && equalY)
            {
                return connection;
            }
        }
    }
    return nullptr;
}

QString NetworkGraphics::getFileName(void)
{
    return _manager->getFileName();
}

/*!
 * \brief Gibt einen Componentport aus der ComponentList aus, welcher sich an den Soll-Position in der Scene befinden.
 *
 * \param   scenePosition ist die Position in der Scene
 * \return Gibt den ComponentPort an der Position zurück.
 *
 * Es wird in der Componentlist nach einem Port an der Sceneposition gesucht.
 * Wenn ein Port gefunden wurde, wird dann ein Componentport erstellt und zurückgegeben.
 * Wenn kein Port gefunden wurde, wird der Nullpointer zurückgegeben
 */
ComponentPort* NetworkGraphics::getComponentPortAtPosition(QPointF scenePosition)
{
    for (GridObject* gridObject : _objects)
    {
        Component* component = dynamic_cast<Component*>(gridObject);
        if (nullptr != component)
        {
            bool hasFoundPort = component->hasPortAtPosition(scenePosition);
            if (hasFoundPort)
            {
                Component::Port port = component->getPort(scenePosition);
                ComponentPort* cp = new ComponentPort(component, port);
                return cp;
            }
        }
    }
    return nullptr;
}

/*!
 * \brief Speichert das erstellte Netzwerk ab.
 */
void NetworkGraphics::save(void)
{
    _manager->save();
    _hasChangedDocument = false;
}

/*!
 * \brief Lädt ein gespeichertes Netzwek.
 */
void NetworkGraphics::load(void)
{
    _isLoading = true;
    _manager->load();
    _isLoading = false;
    _hasChangedDocument = false;

    updateCalc();
    emit resistanceValueChanged();
    if (_objects.count() != 0)
    {
        bool powerSupplyFound = false;
        for (GridObject* gridObject : _objects)
        {
            Component* component = dynamic_cast<Component*>(gridObject);
            if (component != nullptr && (component->getComponentType() == Component::PowerSupply))
            {
                powerSupplyFound = true;
            }
        }

        bool isAllowedPowerSupply = !powerSupplyFound;
        emit powerSupplyIsAllowed(isAllowedPowerSupply);
        emit newNetworkIsLoad();
    }
}

/*!
 * \brief Speichert ein erstelltes Netzwerk ab.
 */
void NetworkGraphics::saveAs(void)
{
    _manager->saveAs();
}

/*!
 * \brief   Spiegelt eine ausgewählte Komponente.
 *
 * \param   component   ist die zu spiegelde Komponente
 */
void NetworkGraphics::mirrorComponent(Component* component)
{
    for (Connection* connection : _connections)
    {
        if (connection->hasComponent(component))
        {
            connection->changePortOfComponentPortWithComponent(component);
        }
    }
}

/*!
 * \brief Erzeugt eine neue Komponente im Netzwerk.
 *
 * \param   gridPosition            ist die zu prüfende Gitterposition
 * \param   componentType           ist der typ der Komponente
 * \param   componentIsVertical     ist die räumliche Ausrichtung der Komponente
 * \return Gibt eine neue Komponente zurück.
 *
 * Zu Beginn wird geprüft ob sich an der ausgewählten Position bereits eine Komponente befindet.
 * Anschließend wird anhand des ausgewählten Typs, jeweils ein Widerstand oder eine Spannungsquelle erzeugt.
 */
Component* NetworkGraphics::addComponent(QPointF gridPosition,
                                         Component::ComponentType componentType, bool componentIsVertical)
{
    CommandAddComponent* addComponent = new CommandAddComponent(this, gridPosition, componentType, componentIsVertical);
    _undoStack->push(addComponent);
    Component* createdComponent = addComponent->getCreatedComponent();

    return createdComponent;
}

/*!
 * \brief Erzeugt eine neue Komponente.
 *
 * \param   gridPosition        ist die Position, an der die Komponente erzeugt werden soll
 * \param   componentType       ist der Typ der Komponente
 * \param   componentIsVertical ist die Ausrichtung der Komponente
 * \return Gibt eine Komponente zurück.
 *
 * Erzeugt entsprechend des componentType die Komponente.
 */
Component* NetworkGraphics::createNewComponentWithoutUndo(QPointF gridPosition,
                                                          Component::ComponentType componentType,
                                                          bool componentIsVertical)
{
    Component* createdComponent = nullptr;

    if (hasObjectAtPosition(gridPosition))
    {
        return nullptr;
    }

    if (Component::ComponentType::Resistor == componentType)
    {

        createdComponent = addResistor("", 100, gridPosition.x(), gridPosition.y(), componentIsVertical);
    }
    else if (Component::ComponentType::PowerSupply == componentType)
    {
        createdComponent = addPowerSupply("", gridPosition.x(), gridPosition.y(), componentIsVertical, 100, 0);

        emit powerSupplyIsAllowed(false);

    }

    if (!_isLoading)
    {
        updateCalc();
    }
    return createdComponent;
}

/*!
 * \brief Dupliziert eine ausgewählte Komponente.
 *
 * \param   componentToDuplicate    ist die zu duplizierende Komponente
 * \param   xPosition               ist die X-Koordinate der übergebenden Komponente
 * \param   yPosition               ist die Y-Koordinate der übergebenden Komponente
 * \return Gibt die duplizierte Komponente zurück.
 *
 * Zuerst wird der Name, Wert und die räumliche Ausrichtung der zu kopierenden Komponente erfragt.
 * Anschließend wird abhängig vom Typ der Komponente ein Widerstand oder ein Spannungsquelle erzeugt.
 * Dann wird der Gesamtwiderstand aktualisiert.
 */
Component* NetworkGraphics::duplicateComponent(Component* componentToDuplicate, int xPosition, int yPosition)
{
    CommandDuplicateComponent* duplicateComponent = new CommandDuplicateComponent(this, componentToDuplicate, xPosition,
                                                                                  yPosition);
    _undoStack->push(duplicateComponent);
    Component* createdComponent = duplicateComponent->getCreatedComponent();

    return createdComponent;
}

/*!
 * \brief Dupliziert eine ausgewählte Komponente.
 *
 * \param   componentToDuplicate    ist die zu duplizierende Komponente
 * \param   xPosition               ist die X-Koordinate der übergebenden Komponente
 * \param   yPosition               ist die Y-Koordinate der übergebenden Komponente
 * \return Gibt die duplizierte Komponente zurück.
 *
 * Zuerst wird der Name, Wert und die räumliche Ausrichtung der zu kopierenden Komponente erfragt.
 * Anschließend wird abhängig vom Typ der Komponente ein Widerstand oder ein Spannungsquelle erzeugt.
 * Dann wird der Gesamtwiderstand aktualisiert.
 */
Component* NetworkGraphics::duplicateComponentWithoutUndo(Component* componentToDuplicate, int xPosition, int yPosition)
{
    Component* duplicatedComponent = nullptr;

    QString name = componentToDuplicate->getName();

    Resistor* resistor = dynamic_cast<Resistor*>(componentToDuplicate);
    bool isResistor = (nullptr != resistor);
    bool componentIsVertical = componentToDuplicate->isVertical();
    if (isResistor)
    {
        long double resistance = resistor->getResistanceValue();
        duplicatedComponent = addResistor(name, resistance, xPosition, yPosition, componentIsVertical);
    }
    else
    {
        double voltage = componentToDuplicate->getVoltage();
        duplicatedComponent = addPowerSupply(name, xPosition, yPosition, componentIsVertical, voltage, 0);
    }

    updateCalc();
    return duplicatedComponent;
}

/*!
 * \brief Verdoppelt ein Textfeld.
 *
 * \param   descriptionToDuplicate  ist das zu verdoppelde Textfeld
 * \param   xPosition               ist die X-Koordinate, an der das Textfeld erzeugt werden soll
 * \param   yPosition               ist die Y-Koordinate, an der das Textfeld erzeugt werden soll
 * \return Gibt ein neues Textfeld zurück.
 */
Description*
NetworkGraphics::duplicateDescription(Description* descriptionToDuplicate, int xPosition, int yPosition)
{
    CommandDuplicateDescription* commandDuplicateDescription = new CommandDuplicateDescription(this,
                                                                                               descriptionToDuplicate,
                                                                                               xPosition, yPosition);
    _undoStack->push(commandDuplicateDescription);
    //return addDescriptionWithoutUndo(QPointF(xPosition, yPosition), false, descriptionToDuplicate->getText());
    return commandDuplicateDescription->getCreatedDescription();
}

/*!
 * \brief Fügt einen Widerstand dem Netzwerk hinzu.
 *
 * \param   name            ist der Name des Widerstandsobjektes
 * \param   valueResistance ist der zugewiesene Widerstandswertes
 * \param   x               ist die X-Koordinate der zugewiesenen Position
 * \param   y               ist die Y-Koordinate der zugewiesenen Position
 * \param   isVertical      ist die räumliche Ausrichtung im Netzwerk
 * \param   id              ist die intern zugewiesene Id des Widerstandes
 * \return Gibt einen neuen Widerstand zurück.
 *
 * Falls noch kein Name vorhanden ist, wird dieser neu erzeugt.
 * Anschließend wird ein neuer Widerstand erzeugt.
 * Es wird nach dem hinzufügen der Widerstandswert neu berechnet.
 */
Component*
NetworkGraphics::addResistor(QString name, long double valueResistance, int xPosition, int yPosition, bool isVertical,
                             int id)
{
    if ("" == name)
    {
        name = "R" + QString::number(_resistorCount + 1);
    }

    if (0 == id)
    {
        //Finden einer freien ID
        int newId = _resistorCount + 1;
        bool isSetId = false;
        while (!isSetId)
        {
            bool isIdValid = true;
            for (GridObject* gridObject : _objects)
            {
                Component* component = dynamic_cast<Component*>(gridObject);
                if (component != nullptr && component->getId() == newId)
                {
                    isIdValid = false;
                }
            }
            if (isIdValid)
            {
                id = newId;
                isSetId = true;
                break;
            }
            else
            {
                newId++;
            }
        }

    }

    Component* resistor = new Resistor(name, valueResistance, xPosition, yPosition,
                                       isVertical, id);
    addComponentWithoutUndo(resistor);
    return resistor;
}

/*!
 * \brief Fügt eine Spannungsquelle dem Netzwerk hinzu.
 *
 * \param   name            ist der Name des Spannungsquellenobjektes
 * \param   x               ist die X-Koordinate der zugewiesenen Position
 * \param   y               ist die Y-Koordinate der zugewiesenen Position
 * \param   isVertical      ist die räumliche Ausrichtung im Netzwerk
 * \param   id              ist die intern zugewiesene Id der Spannungsquelle
 * \return Gibt eine neue Spannungsquelle zurück.
 *
 * Falls noch kein Name vorhanden ist, wird dieser neu erzeugt.
 * Befindet sich im Netzwerk bereits eine Spannungsquelle, kann keine weitere erzeugt werden.
 * Anschließend wird eine neue Spannungsquelle erzeugt.
 */
Component* NetworkGraphics::addPowerSupply(QString name, int x, int y, bool isVertical, double voltage, int id)
{
    if ("" == name)
    {
        name = "Q" + QString::number(_powerSupplyCount);
    }

    Component* powerSupply = new PowerSupply(name, x, y,
                                             isVertical, voltage, id);
    addComponentWithoutUndo(powerSupply);
    return powerSupply;
    return nullptr;
}

/*!
 * \brief Schneidet ausgewählte Komponenten aus.
 *
 * \param componentToCut ist die Komponente die Ausgeschnitten werden soll
 */
void NetworkGraphics::cutComponentWithoutUndo(Component* componentToCut)
{
    if (nullptr != componentToCut)
    {
        removeItem(componentToCut);
        _objects.removeOne(componentToCut);
        if (componentToCut->getComponentType() == Component::PowerSupply)
        {
            emit powerSupplyIsAllowed(true);
            _powerSupplyCount--;
        }
        update();
    }
}

void NetworkGraphics::cutDescriptionWithoutUndo(Description* descriptionToCut)
{
    if (nullptr != descriptionToCut)
    {
        removeItem(descriptionToCut);
        _objects.removeOne(descriptionToCut);
        update();
    }
}

/*!
 * \brief Erzeugt ein neues Textfeld.
 *
 * \param   gridPosition    ist die Gitterposition innerhalb im Netzwerk
 * \param   isLoad          ist ein bool der angibt, ob das Textfeld bereits geladen wurde
 * \param   text            ist der Text des Textfeldes
 * \param   id              ist die Id des Textfeldes
 * \return Gibt ein neues Textfeld zurück.
 *
 * Es wird zuerst geprüft ob das Textfeld bereits geladen wurde.
 * Wenn das Textfeld noch nicht geladen wurde, wird geprüft, ob sich an der Gitterposition bereits eine Komponente oder ein Textfeld befindet.
 * Dann wird die Id des Textfeldes gesetzt.
 * Wenn das Textfeld bereits geladen wurde entfallen die beiden oberen Schritte.
 * Anschließend wird ein neues Textfeld erzeugt und der descriptionCount um eins erhöht.
 */
Description*
NetworkGraphics::addDescriptionWithoutUndo(QPointF gridPosition, bool isLoad, [[maybe_unused]] QString text,
                                           [[maybe_unused]] int id)
{
    if (!isLoad)
    {
        if (hasObjectAtPosition(gridPosition))
        {
            return nullptr;
        }
        id = _descriptionCount;
    }

    Description* description = new Description(gridPosition.x(), gridPosition.y(), id, text);
    addDescriptionWithoutUndo(description);

    return description;
}

/*!
 * \brief Fügt ein Textfeld dem Netzwerk hinzu.
 *
 * \param   descriptionFieldToAdd   ist das Textfeld, welches hinzugefügt wird
 *
 */
void NetworkGraphics::addDescriptionWithoutUndo(Description* descriptionFieldToAdd)
{
    _descriptionCount++;
    addObject(descriptionFieldToAdd);
}

/*!
 * \brief Entfernt Komponenten aus dem Netzwerk.
 *
 * \param   component ist die zu entfernende Komponente
 *
 * Wenn die ausgewählte Komponente vorhanden ist, wird diese entfernt.
 * Nach dem entfernen aus dem Netzwerk wird die Komponente aus dem componentList entfernt.
 * Wenn es sich bei der Komponente um einen Widerstand handelt wird der resistorCount um 1 reduziert.
 * Wenn es sich um eine Spannungsquellle handelt geschieht dies analog zum Widerstand.
 * Anschließend werden die Verbindungen, die sich an der Komponente befinden entfernt.
 * Zum Schluss wird der Widerstandswert neu berechnet.
 */
QList<Connection*> NetworkGraphics::deleteComponentWithoutUndoAndGetDeletedConnections(Component* component)
{
    QList<Connection*> deletedConnections;
    if (component != nullptr)
    {
        removeItem(component);
        _objects.removeOne(component);

        //ResistorCount und PowerSupplyCount setzen
        if (Component::ComponentType::Resistor == component->getComponentTypeInt())
        {
            _resistorCount--;
        }
        else if (Component::ComponentType::PowerSupply == component->getComponentTypeInt())
        {
            _powerSupplyCount--;
            emit powerSupplyIsAllowed(true);
        }

        //Lösche Verbindungen vom Component
        for (Connection* connection : _connections)
        {
            if (connection->hasComponent(component))
            {
                removeItem(connection);
                _connections.removeOne(connection);
                deletedConnections.append(connection);
            }
        }
        updateCalc();
    }
    update();
    return deletedConnections;
}

/*!
 * \brief Entfernt ein Textfeld aus dem Netzwerk.
 *
 * \param   description ist das zu entfernende Textfeld
 *
 * Wenn das ausgewählte Textfeld vorhanden ist, wird diese entfernt.
 * Dannach wird das Textfeld aus der Liste descriptions entfernt.
 * Der descriptionCount wird um eins reduziert.
 */
void NetworkGraphics::deleteDescriptionWithoutUndo(Description* description)
{
    if (description != nullptr)
    {
        removeItem(description);
        _objects.removeOne(description);
        _descriptionCount--;
    }
}

/*!
 * \brief Entfernt eine Verbindung aus dem Netzwerk.
 *
 * \param   connecton ist die zu entfernende Verbindung
 *
 * Wenn die ausgewählte Verbindung vorhanden ist, wird diese entfernt.
 * Nach dem entfernen aus dem Netzwerk wird die Verbindung aus der connectionList entfernt.
 */
void NetworkGraphics::deleteConnectionWithoutUndo(Connection* connection)
{
    if (connection != nullptr)
    {
        removeItem(connection);
        _connections.removeOne(connection);
    }

    updateCalc();
    update();
}

/*!
 * \brief Verschiebt eine Komponente im Netzwerk.
 *
 * \param   objectToMove    ist die zu verschiebende Komponente
 * \param   descriptionToMove  ist die zu verschiebende Beschreibung
 * \param   gridPosition       ist die Position, an die die Komponente verschoben werden soll
 *
 * Es wird zuerst geprüft ob sich an der neuen Gitterposition bereits eine Komponente oder ein Textfeld befindet.
 * Befindet sich an der Position nichts, wird die ausgewählte Komponente an die neu Position verschoben.
 */
void
NetworkGraphics::moveComponentWithoutUndo(GridObject* objectToMove, QPointF gridPosition)
{
    bool isComponentAtPosition = hasObjectAtPosition(gridPosition);
    if (isComponentAtPosition)
    {
        return;
    }
    if (objectToMove != nullptr)
    {
        objectToMove->setPosition(gridPosition);
    }
    update();
}

/*!
 * \brief Dreht die Komponente linksrum.
 *
 * \param   componentToTurn ist die zu drehende Komponente
 *
 * Es wird die Methode turnComponentRight drei Mal aufgerufen, da dies einmal linksrum drehen entspricht.
 */
void NetworkGraphics::turnComponentLeft(Component* componentToTurn)
{
    // 3 mal rechts ist einmal links
    for (int i = 0; i < 3; i++)
    {
        turnComponentRight(componentToTurn);
    }
}

/*!
 * \brief Dreht die Komponente rechtsrum.
 *
 * \param   componentToTurn ist die zu drehende Komponente
 *
 * Anhand der Ausrichtung der Komponente, wird die neue Ausrichtung nach einmaligem rechtsrum drehen gesetzt.
 */
void NetworkGraphics::turnComponentRight(Component* componentToTurn)
{
    CommandRotateComponent* commandRotateComponent = new CommandRotateComponent(componentToTurn, this);
    _undoStack->push(commandRotateComponent);
}

/*!
 * \brief Setzt die Orientierung einer ausgewählten Komponente.
 *
 * \param   componentToTurn ist die zu drehende Komponente
 * \param   orientation     ist die Orientierung der zu drehenden Komponente
 *
 * Die Komponente wird so lange rechtsrum gedreht, bis die neue Orientierung der übergebenen Orientierung ist.
 */
void NetworkGraphics::setOrientationOfComponent(Component* componentToTurn, Component::Orientation orientation)
{
    while (componentToTurn->getOrientation() != orientation)
    {
        turnComponentRight(componentToTurn);
    }
}

/*!
 * \brief Aktualisiert den Widerstandswert.
 *
 * Die aktuellen Verbindungen und Komponenten werden dem Calculator übergeben.
 * Anschließend wird der aktualisierte Widerstandswert ausgegeben.
 */
void NetworkGraphics::updateCalc(void)
{
    if (!_isLoading)
    {
        _resistanceValue = Calculator::calculator().calculate(_connections, getComponents());

        update();
        emit resistanceValueChanged();
        if (_resistanceValue != 0)
        {
            emit currentAndVoltageIsValid(true);
        }
        else
        {
            emit currentAndVoltageIsValid(false);
        }
    }
}

/*!
 * \brief Fügt eine Verbindung der Schaltung hinzu.
 *
 * \param   componentPortA ist der Startpunkt der Verbindung
 * \param   componentPortB ist der Endpunkt der Verbindung
 *
 * Fügt eine Verbindung zwischen den beiden Ports hinzu.
 */
void NetworkGraphics::addConnection(ComponentPort componentPortA, ComponentPort componentPortB)
{
    CommandAddConnection* commandAddConnection = new CommandAddConnection(this, componentPortA, componentPortB);
    _undoStack->push(commandAddConnection);
}

/*!
 * \brief Verschiebt eine Komponente.
 *
 * \param   objectToMove     ist die zu verschiebene Komponente
 * \param   descriptionToMove   ist das zu verschieben Textfeld
 * \param   gridPosition        neue Position
 *
 * Wenn eine Komponente ausgewählt ist, wird die Komponente verschoben.
 */
void
NetworkGraphics::moveObject(GridObject* objectToMove, QPointF gridPosition)
{
    if (objectToMove != nullptr)
    {
        CommandMoveComponent* commandMoveComponent = new CommandMoveComponent(this, objectToMove,
                                                                              gridPosition);
        _undoStack->push(commandMoveComponent);
    }
}

/*!
 * \brief Bearbeiten eine Komponente.
 *
 * \param componentToEdit   ist die zu bearbeitende Komponente
 * \param newName           ist der neue Name der zu bearbeitenden Komponente
 * \param newValue          ist der neue Wert der zu bearbeitenden Komponente
 */
void NetworkGraphics::editComponentWithoutUndo(Component* componentToEdit, QString newName, long double newValue)
{
    componentToEdit->setName(newName);
    Resistor* resistor = dynamic_cast<Resistor*>(componentToEdit);
    bool isResistor = (nullptr != resistor);
    if (isResistor)
    {
        resistor->setResistanceValue(newValue);
    }
    else
    {
        componentToEdit->setVoltage(newValue);
    }

    updateCalc();
}

/*!
 * \brief Dreht eine ausgewählte Komponente.
 *
 * \param componentToTurn ist die zu drehende Komponente
 */
void NetworkGraphics::turnComponentRightWithoutUndo(Component* componentToTurn)
{
    switch (componentToTurn->getOrientation())
    {
        case Component::Orientation::left:
        {
            componentToTurn->setOrientation(Component::Orientation::top);
        }
            break;
        case Component::Orientation::top:
        {
            componentToTurn->setOrientation(Component::Orientation::right);
            mirrorComponent(componentToTurn);
        }
            break;
        case Component::Orientation::right:
        {
            componentToTurn->setOrientation(Component::Orientation::bottom);
        }
            break;
        case Component::Orientation::bottom:
        {
            componentToTurn->setOrientation(Component::Orientation::left);
            mirrorComponent(componentToTurn);
        }
            break;
    }
    update();
}

/*!
 * \brief Bearbeiten einer Komponente.
 *
 * \param   componentToEdite    ist die zu bearbeitende Komponente
 * \param   newName             ist der neue name für die Komponente
 * \param   newValue            ist der neue Widerstandswert
 * \param   originalOrientation ist die vorherige ausrichtung
 */
void NetworkGraphics::editComponent(Component* componentToEdit, QString newName, double newValue,
                                    Component::Orientation originalOrientation)
{
    CommandEditComponent* commandEditComponent = new CommandEditComponent(this, componentToEdit, originalOrientation,
                                                                          newName, newValue);
    _undoStack->push(commandEditComponent);
}

/*!
 * \brief Fügt eine Komponente hinzu.
 *
 * \param   componentToAdd  ist die hinzuzufügende Komponente
 *
 * Fügt eine Komponente an einer Position hinzu, wenn sich an dieser keine befindet.
 */
void NetworkGraphics::addComponentWithoutUndo(Component* componentToAdd)
{
    QPointF gridPosition = componentToAdd->getPosition();
    if (hasObjectAtPosition(gridPosition))
    {
        return;
    }

    Component::ComponentType componentType = componentToAdd->getComponentType();
    if (Component::ComponentType::Resistor == componentType)
    {
        _resistorCount++;
        addObject(componentToAdd);
    }
    else if (Component::ComponentType::PowerSupply == componentType)
    {
        if (_powerSupplyCount < 1)
        {
            _powerSupplyCount++;
            addObject(componentToAdd);
            emit powerSupplyIsAllowed(false);
        }
        else
        {
            QMessageBox::about(nullptr, "Fehleingabe", "Nur eine Spannungsquelle erlaubt");
        }
    }

    updateCalc();
}

/*!
 * \brief Entfernt eine Komponente.
 *
 * \param   componentToDelete   ist die zu entfernende Komponete
 */
void NetworkGraphics::deleteComponent(Component* componentToDelete)
{
    CommandDeleteComponent* commandDeleteComponent = new CommandDeleteComponent(this, componentToDelete);
    _undoStack->push(commandDeleteComponent);
}

/*!
 * \brief Fügt eine Verbindung hinzu.
 *
 * \param   connection   ist die Verbindung, die hinzugefügt wird
 *
 * Es wird geprüft, ob die Verbindung bereits existiert.
 * Anschließend wird die Verbindung hinzugefügt.
 */
void NetworkGraphics::addConnectionWithoutUndo(Connection* connection)
{
    bool isAlreadyExisting = false;
    for (Connection* otherConnection : _connections)
    {
        if (*otherConnection == *connection)
        {
            isAlreadyExisting = true;
            break;
        }
    }
    if (!isAlreadyExisting && nullptr != connection)
    {
        _connections.append(connection);
        addItem(connection);
    }

    updateCalc();
}

/*!
 * \brief Entfernt eine ausgewählte Verbindung.
 *
 * \param   connectionToDelete  ist die zu entfernende Verbindung
 */
void NetworkGraphics::deleteConnection(Connection* connectionToDelete)
{
    CommandDeleteConnection* commandDeleteConnection = new CommandDeleteConnection(this, connectionToDelete);
    _undoStack->push(commandDeleteConnection);
}

/*!
 * \brief Entfernt ein Textfeld.
 *
 * \param   descriptionFieldToDelete    ist das zu entfernende Textfeld
 */
void NetworkGraphics::deleteDescription(Description* descriptionFieldToDelete)
{
    CommandDeleteDescription* commandDeleteDescription = new CommandDeleteDescription(this, descriptionFieldToDelete);
    _undoStack->push(commandDeleteDescription);
}

/*!
 * \brief Bearbeit ein Textfeldes.
 *
 * \param descriptionToEdit ist das zu bearbeitende Textfeld
 * \param newText           ist der neue Text des Textfeldes
 */
void NetworkGraphics::editDescription(Description* descriptionToEdit, QString newText)
{
    CommandEditDescription* commandEditDescription = new CommandEditDescription(this, descriptionToEdit, newText);
    _undoStack->push(commandEditDescription);
}

/*!
 * \brief Verschiebt zusammengefasste Komponente an eine andere Gitterposition.
 *
 * \param objects     ist die Liste der zusammengefassten Objekte
 * \param objectToMove   ist die ausgewählte Komponente
 * \param descriptionToMove ist das ausgewählte Textfeld
 * \param diffXAfterMoving  ist die Differenz zwischen der Ausgangs und der End X-Koordinate
 * \param diffYAfterMoving  ist die Differenz zwischen der Ausgangs und der End Y-Koordinate
 */
void
NetworkGraphics::moveMultiselectComponents(QList<GridObject*> objects, GridObject* objectToMove,
                                           int diffXAfterMoving, int diffYAfterMoving)
{
    bool terminationCondition = false;
    QList<GridObject*> notMovedObjects;
    for (GridObject* gridObject : objects)
    {
        if (gridObject->isSelected() && gridObject != objectToMove)
        {
            int xPosition = gridObject->getPosition().x();
            int yPosition = gridObject->getPosition().y();

            QPointF* newPosition = new QPointF(xPosition + diffXAfterMoving,
                                               yPosition + diffYAfterMoving);
            if (hasObjectAtPosition(*newPosition))
            {
                if (getObjectAtPosition(*newPosition) != nullptr &&
                    !getObjectAtPosition(*newPosition)->isSelected())
                {
                    terminationCondition = true;
                }
                notMovedObjects.append(gridObject);
            }
            moveObject(gridObject, *newPosition);
        }
    }

    if (terminationCondition)
    {
        return;
    }
    if (!notMovedObjects.isEmpty())
    {
        moveMultiselectComponents(notMovedObjects, objectToMove, diffXAfterMoving, diffYAfterMoving);
    }
}

/*!
 * \brief Schneidet eine ausgewählte Komponente aus.
 *
 * \param componentToCut ist die ausgewählte Komponente, die ausgeschnitten werden soll
 */
void NetworkGraphics::cutComponent(Component* componentToCut)
{
    CommandCutComponents* commandCutComponent = new CommandCutComponents(this, componentToCut);
    _undoStack->push(commandCutComponent);
}

void NetworkGraphics::cutDescription(Description* descriptionToCut)
{
    CommandCutDescriptionField* commandCutDescriptionField = new CommandCutDescriptionField(this, descriptionToCut);
    _undoStack->push(commandCutDescriptionField);
}

/*!
 * \brief Fügt ein Textfeld hinzu.
 *
 * \param   gridPosoition   ist die GitterPositon, an der das Textfeld erzeugt werden soll
 * \param   isLoad          ob das Textfeld aus einer Datei geladen wurde
 * \param   text            der Textinhalt des Textfeldes
 * \param   id              die intern vergebene Id
 * \return Gibt ein Textfeld zurück.
 *
 * Wenn das Textfeld nicht geladen wird, wird überprüft, ob sich eine Komponente oder ein Textfeld an der Position befindet und die Id neu vergeben.
 * Anschließend wird das Textfeld erstellt.
 */
Description* NetworkGraphics::addDescriptionField(QPointF gridPosition, bool isLoad, QString text, int id)
{
    if (!isLoad)
    {
        if (hasObjectAtPosition(gridPosition))
        {
            return nullptr;
        }
        id = _descriptionCount;
    }

    Description* description = new Description(gridPosition.x(), gridPosition.y(), id, text);

    CommandAddDescriptionField* commandAddDescriptionField = new CommandAddDescriptionField(this, description);
    _undoStack->push(commandAddDescriptionField);

    return description;
}

QString NetworkGraphics::getVoltageAndCurrentInformation(void)
{
    QString information;
    if (!Calculator::calculator().hasUsedStarCalculation())
    {
        for (Component* c : getComponents())
        {
            if (c->getComponentType() == Component::PowerSupply)
            {
                information +=
                        "Gesamtspannung: " + QString::number(c->getVoltage(), 'f', 2) + "V" + "<br>" + "Gesamtstrom: " +
                        QString::number(c->getAmp(), 'f', 2)
                        + "A" + "<br> <br>";
            }
        }
        for (Component* c : getComponents())
        {
            if (c->getComponentType() == Component::Resistor)
            {
                Resistor* resistor = dynamic_cast<Resistor*>(c);
                bool isResistor = (nullptr != resistor);
                if (isResistor)
                {
                    information += c->getName() + ": <br>";
                    information += "Widerstand: ";
                    information += QString::number(resistor->getResistanceValue(), 'f', 2) + "Ω" + "<br>";
                    information += "Strom: ";
                    information += QString::number(c->getAmp(), 'f', 2) + "A" + "<br>";
                    information += "Spannung: ";
                    information += QString::number(c->getVoltage(), 'f', 2) + "V";
                    information += " <br> <br>";
                }
            }
        }
        information += "Gesamtwiderstand: " + QString::number(getResistanceValue(), 'f', 2) + "Ω";
    }
    else
    {
        information += "Warnung: <br><br>";
        information += "In Ihrem Netzwerk wurde eine Stern-Dreiecks-Umformung durchgeführt."
                       "<br>Da sich die Funktion der Wertetabelle noch in der Beta befindet, können wir Ihnen keine richtigen Spannungen und Ströme ausgeben."
                       "<br><br> Vielen Dank für Ihr Vertändnis!";
    }
    return information;
}

void NetworkGraphics::hasChangedDocument([[maybe_unused]]int idx)
{
    _hasChangedDocument = true;
}

bool NetworkGraphics::hasChangedDocument(void)
{
    return _hasChangedDocument;
}

QList<GridObject*> NetworkGraphics::getSelectedObjects(void)
{
    QList<GridObject*> selectedObjects;
    for (GridObject* gridObject : _objects)
    {
        if (gridObject->isSelected())
        {
            selectedObjects.append(gridObject);
        }
    }
    return selectedObjects;
}

void NetworkGraphics::deselectAllItems(void)
{
    for (GridObject* gridObject : _objects)
    {
        gridObject->setSelected(false);
    }

    for (Connection* connection : getConnections())
    {
        connection->setSelected(false);
    }

    update();
}

void NetworkGraphics::selectObjectsAtPosition(QPointF scenePosition)
{
    GridObject* foundObject = getObjectAtPosition(scenePosition);
    Connection* foundConnection = getConnectionAtPosition(scenePosition);

    bool hasFoundObject = nullptr != foundObject;
    bool hasFoundConnection = nullptr != foundConnection;

    if (hasFoundObject)
    {
        foundObject->setSelected(true);
    }
    else if (hasFoundConnection)
    {
        foundConnection->setSelected(true);
    }
}

QPointF NetworkGraphics::mapSceneToGrid(QPointF scenePosition)
{
    qreal xPos = scenePosition.toPoint().x() / Defines::gridLength * Defines::gridLength - (Defines::gridLength / 2);
    qreal yPos = scenePosition.toPoint().y() / Defines::gridLength * Defines::gridLength - (Defines::gridLength / 2);
    return QPointF(xPos, yPos);
}

QList<Component*> NetworkGraphics::findSelectedComponents(void)
{
    QList<Component*> componentList;
    for (Component* component : getComponents())
    {
        if (component->isSelected())
        {
            componentList.append(component);
        }
    }
    return componentList;
}

QList<Description*> NetworkGraphics::findSelectedDescription(void)
{
    QList<Description*> descriptionList;
    for (Description* description : getDescriptions())
    {
        if (description->isSelected())
        {
            descriptionList.append(description);
        }
    }
    return descriptionList;
}

void NetworkGraphics::cutComponent(QList<Component*> components)
{
    for (Component* componentToCut : components)
    {
        cutComponent(componentToCut);
    }
}

void NetworkGraphics::cutDescription(QList<Description*> descriptions)
{
    for (Description* descriptionToCut : descriptions)
    {
        cutDescription(descriptionToCut);
    }
}

void NetworkGraphics::turnSelectedComponentsRight(void)
{
    QList<Component*> selectedComponents = findSelectedComponents();
    for (Component* component : selectedComponents)
    {
        turnComponentRight(component);
    }
}

void NetworkGraphics::selectObjectsInArea(QRectF selectionArea)
{
    for (GridObject* gridObject : _objects)
    {
        bool isInSelectionArea = selectionArea.intersects(gridObject->boundingRect());
        gridObject->setSelected(isInSelectionArea);
    }
    update();
}

QList<Component*> NetworkGraphics::getComponents(void)
{
    QList<Component*> components;
    for (GridObject* gridObject : _objects)
    {
        Component* component = dynamic_cast<Component*>(gridObject);
        if (nullptr != component)
        {
            components.append(component);
        }
    }
    return components;
}

QList<Description*> NetworkGraphics::getDescriptions(void)
{
    QList<Description*> descriptions;
    for (GridObject* gridObject : _objects)
    {
        Description* component = dynamic_cast<Description*>(gridObject);
        if (nullptr != component)
        {
            descriptions.append(component);
        }
    }
    return descriptions;
}
