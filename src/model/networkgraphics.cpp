#include "defines.h"
#include "model/networkgraphics.h"
#include "model/calculator.h"

#include <QDebug>

NetworkGraphics::NetworkGraphics() : QGraphicsScene(), _graphics(new QGraphicsScene()), _undoStack(new QUndoStack(this))
{
    // die Anfangsgröße wird initialisiert
    int defaultSceneSize = Defines::defaultSceneSize;
    setSceneRect(-defaultSceneSize, -defaultSceneSize, defaultSceneSize, defaultSceneSize);
    _manager = new FileManager(this);
    connect(_undoStack, SIGNAL(indexChanged(int)), this, SLOT(hasChangedDocument(int)));
}

/*!
 * \brief Fügt eine Verbindung dem Netzwerk hinzu.
 *
 * \param   componentPortA  Port der Komponente 1.
 * \param   componentPortB  Port der Komponente 2.
 * \return  Gibt die Connection zwischen den ComponentPorts zurück.
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
 * \brief Liefert ein GridObject, welches an der Soll-Position ist.
 *
 * \param   scenePosition   ist die zu überprüfende Gitterposition
 * \return  Gibt ein das GridObject an der Position zurück.
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
    _hasChangedDocument = false;
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
 * \param   scenePosition            ist die zu prüfende Gitterposition
 * \param   componentType           ist der typ der Komponente
 * \param   componentIsVertical     ist die räumliche Ausrichtung der Komponente
 * \return Gibt eine neue Komponente zurück.
 *
 * Zu Beginn wird geprüft ob sich an der ausgewählten Position bereits eine Komponente befindet.
 * Anschließend wird anhand des ausgewählten Typs, jeweils ein Widerstand oder eine Spannungsquelle erzeugt.
 */
Component* NetworkGraphics::addComponent(QPointF scenePosition,
                                         Component::ComponentType componentType, bool componentIsVertical)
{
    QPointF gridPosition = mapSceneToGrid(scenePosition);
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
    QPointF gridPosition = mapSceneToGrid(QPointF(xPosition, yPosition));
    CommandDuplicateComponent* duplicateComponent = new CommandDuplicateComponent(this, componentToDuplicate, gridPosition.x(),
                                                                                  gridPosition.y());
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
    QPointF gridPosition = mapSceneToGrid(QPointF(xPosition, yPosition));
    CommandDuplicateDescription* commandDuplicateDescription = new CommandDuplicateDescription(this,
                                                                                               descriptionToDuplicate,
                                                                                               gridPosition.x(), gridPosition.y());
    _undoStack->push(commandDuplicateDescription);
    Description* createdDescription = commandDuplicateDescription->getCreatedDescription();
    return createdDescription;
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
    powerSupply = addComponentWithoutUndo(powerSupply);
    return powerSupply;
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
 */
void NetworkGraphics::addDescriptionWithoutUndo(Description* descriptionFieldToAdd)
{
    _descriptionCount++;
    addObject(descriptionFieldToAdd);
}

/*!
 * \brief Entfernt Komponenten aus dem Netzwerk.
 *
 * \param   componentToDelete ist die zu entfernende Komponente
 *
 * Wenn die ausgewählte Komponente vorhanden ist, wird diese entfernt.
 * Nach dem entfernen aus dem Netzwerk wird die Komponente aus dem componentList entfernt.
 * Wenn es sich bei der Komponente um einen Widerstand handelt wird der resistorCount um 1 reduziert.
 * Wenn es sich um eine Spannungsquellle handelt geschieht dies analog zum Widerstand.
 * Anschließend werden die Verbindungen, die sich an der Komponente befinden entfernt.
 * Zum Schluss wird der Widerstandswert neu berechnet.
 */
QList<Connection*> NetworkGraphics::deleteComponentWithoutUndoAndGetDeletedConnections(Component* componentToDelete)
{
    QList<Connection*> deletedConnections;
    if (componentToDelete != nullptr)
    {
        removeItem(componentToDelete);
        _objects.removeOne(componentToDelete);
        componentToDelete->setSelected(false);

        //ResistorCount und PowerSupplyCount setzen
        if (Component::ComponentType::Resistor == componentToDelete->getComponentTypeInt())
        {
            _resistorCount--;
        }
        else if (Component::ComponentType::PowerSupply == componentToDelete->getComponentTypeInt())
        {
            _powerSupplyCount--;
            emit powerSupplyIsAllowed(true);
        }

        //Lösche Verbindungen vom Component
        for (Connection* connection : _connections)
        {
            if (connection->hasComponent(componentToDelete))
            {
                removeItem(connection);
                deletedConnections.append(connection);
            }
        }
        for (Connection* connectionToDelete : deletedConnections)
        {
            if (_connections.contains(connectionToDelete))
            {
                _connections.removeOne(connectionToDelete);
            }
        }
    }

    updateCalc();
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
        description->setSelected(false);
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
        connection->setSelected(false);
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
        currentAndVoltageIsValid(_resistanceValue != 0);
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
 * \param   scenePosition        neue Position
 *
 * Wenn eine Komponente ausgewählt ist, wird die Komponente verschoben.
 */
void
NetworkGraphics::moveObject(GridObject* objectToMove, QPointF scenePosition)
{
    QPointF gridPosition = mapSceneToGrid(scenePosition);
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
Component* NetworkGraphics::addComponentWithoutUndo(Component* componentToAdd)
{
    QPointF gridPosition = componentToAdd->getPosition();
    if (hasObjectAtPosition(gridPosition))
    {
        return nullptr;
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
            componentToAdd = nullptr;
        }
    }

    updateCalc();
    return componentToAdd;
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
 * \param selectedObjects     ist die Liste der zusammengefassten Objekte
 * \param objectToMove   ist die ausgewählte Komponente
 * \param descriptionToMove ist das ausgewählte Textfeld
 * \param diffXAfterMoving  ist die Differenz zwischen der Ausgangs und der End X-Koordinate
 * \param diffYAfterMoving  ist die Differenz zwischen der Ausgangs und der End Y-Koordinate
 */
void
NetworkGraphics::moveMultiselectObjects(QList<GridObject*> selectedObjects, GridObject* objectToMove,
                                        QPointF scenePosition)
{
    if (objectToMove != nullptr)
    {
        int previousComoponentXPosition = objectToMove->getPosition().x();
        int previousComoponentYPosition = objectToMove->getPosition().y();

        moveObject(objectToMove, scenePosition);

        int diffX = objectToMove->getPosition().x() - previousComoponentXPosition;
        int diffY = objectToMove->getPosition().y() - previousComoponentYPosition;

        moveObjects(selectedObjects, objectToMove, diffX, diffY);
    }
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
Description* NetworkGraphics::addDescriptionField(QPointF scenePosition, bool isLoad, QString text, int id)
{
    QPointF gridPosition = mapSceneToGrid(scenePosition);
    if (!isLoad)
    {
        if (hasObjectAtPosition(scenePosition))
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

/*!
 * \brief Liefert die Spannungsabfälle und die Ströme.
 *
 * \return Gibt die Stöme und Spannungen zürück.
 */
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

/*!
 * \brief Wählt alle makierten Objekte ab.
 */
void NetworkGraphics::deselectAllItems(void)
{
    deselctAllGridObjects();
    deselectAllConnections();
    update();
}

QPointF NetworkGraphics::mapSceneToGrid(QPointF scenePosition)
{
    qreal xPos = scenePosition.toPoint().x() / Defines::gridLength * Defines::gridLength - (Defines::gridLength / 2);
    qreal yPos = scenePosition.toPoint().y() / Defines::gridLength * Defines::gridLength - (Defines::gridLength / 2);
    return QPointF(xPos, yPos);
}

QList<Component*> NetworkGraphics::getSelectedComponents(void)
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

/*!
 * \brief Dreht alle ausgewählten Komponenten rechtsrum.
 */
void NetworkGraphics::turnSelectedComponentsRight(void)
{
    QList<Component*> selectedComponents = getSelectedComponents();
    for (Component* component : selectedComponents)
    {
        turnComponentRight(component);
    }
}

/*!
 * \brief Wählt alle Objekte in einem Bereich aus.
 *
 * \param selectionArea ist der ausgewählte Bereich
 */
void NetworkGraphics::selectObjectsInArea(QRectF selectionArea)
{
    QList<GridObject*> foundObjects = getGridObjectsInArea(selectionArea);
    for (GridObject* gridObject : foundObjects)
    {
        gridObject->setSelected(true);
    }

    bool hasFoundObjects = !foundObjects.isEmpty();
    if (hasFoundObjects)
    {
        deselectAllConnections();
    }
    else
    {
        deselctAllGridObjects();
        for (Connection* connection : getConnectionsInArea(selectionArea))
        {
            connection->setSelected(true);
        }
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

/*!
 * \brief Liefert die Komponente zu der gesuchten Id.
 *
 * \param id ist die gegebene Id
 * \return Gibt die Komponente zu der Id zurück.
 */
Component* NetworkGraphics::getComponentById(int id)
{
    for (Component* component : getComponents())
    {
        if (component->getId() == id)
        {
            return component;
        }
    }
    return nullptr;
}

void NetworkGraphics::deselectAllConnections(void)
{
    for (Connection* connection : _connections)
    {
        connection->setSelected(false);
    }
    update();
}

QList<GridObject*> NetworkGraphics::getGridObjectsInArea(QRectF selectionArea)
{
    QList<GridObject*> gridObjects;
    for (GridObject* gridObject : _objects)
    {
        bool isInSelectionArea = selectionArea.intersects(gridObject->boundingRect());
        if (isInSelectionArea)
        {
            gridObjects.append(gridObject);
        }
    }
    return gridObjects;
}

QList<Connection*> NetworkGraphics::getConnectionsInArea(QRectF selectionArea)
{
    QList<Connection*> connections;
    for (Connection* connection : _connections)
    {
        for (QRect connectionHitBox : connection->getHitBoxes())
        {
            bool isInSelectionArea = selectionArea.intersects(connectionHitBox);
            if (isInSelectionArea)
            {
                connections.append(connection);
            }
        }
    }
    return connections;
}

void NetworkGraphics::deselctAllGridObjects(void)
{
    for (GridObject* gridObject : _objects)
    {
        gridObject->setSelected(false);
    }
    update();
}

/*!
 * \brief Wählt alle makierten Objekte aus.
 *
 * es werden alle GridObjekte ausgewählt und alle Connections abgewählt.
 */
void NetworkGraphics::selectAll(void)
{
    deselectAllConnections();
    selectAllGridObjects();
}

void NetworkGraphics::selectAllGridObjects(void)
{
    for (GridObject* gridObject : _objects)
    {
        gridObject->setSelected(true);
    }
    update();
}

/*!
 * \brief Schneidet die ausgewählten Objekte aus.
 *
 * \param objectsToCut sind die Objekte, die ausgeschnitten werden sollen
 */
void NetworkGraphics::cutObjects(QList<GridObject*> objectsToCut)
{
    for (GridObject* gridObject : objectsToCut)
    {
        Component* component = dynamic_cast<Component*>(gridObject);
        Description* description = dynamic_cast<Description*>(gridObject);

        if (nullptr != component)
        {
            deleteComponent(component);
        }
        else if (nullptr != description)
        {
            deleteDescription(description);
        }
    }
}

/*!
 * \brief Dupliziert alle ausgewählten Objekte.
 */
void NetworkGraphics::duplicateSelectedGridObjects(void)
{
    duplicateGridObjects(getSelectedObjects());
}

bool NetworkGraphics::lookingForFreeSpaceToDuplicate(int xPos, int yPos, int &xWaytoTheRight)
{
    bool foundUnusedSpace = false;
    while (true)
    {
        if (!hasObjectAtPosition(  QPointF(xPos + xWaytoTheRight, yPos)))
        {
            foundUnusedSpace = true;
            return foundUnusedSpace;
        }

        xWaytoTheRight += Defines::gridLength;
    }
}

/*!
 * \brief Verdoppelt das ausgewählte GridObject.
 *
 * \param gridObjectToDuplicate ist das zu duplizierende Objekt
 * \param xPosition             ist die X-Koordinate, an der das Objekt dupliziert werden soll
 * \param yPosition             ist die Y-Koordinate, an der das Objekt dupliziert werden soll
 * \return Gibt das duplizierte Objekt zurück.
 */
GridObject* NetworkGraphics::duplicateGridObject(GridObject* gridObjectToDuplicate, int xPosition, int yPosition)
{
    GridObject* duplicatedObject = nullptr;
    Component* componentToDuplicate = dynamic_cast<Component*>(gridObjectToDuplicate);
    Description* descriptionToDuplicate = dynamic_cast<Description*>(gridObjectToDuplicate);

    if (nullptr != componentToDuplicate)
    {
        duplicatedObject = duplicateComponent(componentToDuplicate, xPosition, yPosition);
    }
    else if (nullptr != descriptionToDuplicate)
    {
        duplicatedObject = duplicateDescription(descriptionToDuplicate, xPosition, yPosition);
    }
    return duplicatedObject;
}

/*!
 * \brief Entfernt die ausgewählten Objekte.
 *
 * \param copiedObjects ist das zu entfernende Objekt
 */
void NetworkGraphics::deleteSelectedObjects(QList<GridObject*>& copiedObjects)
{
    deleteSelectedGridObjects(copiedObjects);
    deleteSelectedConnections();
    deselectAllItems();
}

void NetworkGraphics::deleteSelectedGridObjects(QList<GridObject*>& copiedObjects)
{
    for (GridObject* selectedObject : getSelectedObjects())
    {
        if (copiedObjects.contains(selectedObject))
        {
            copiedObjects.removeOne(selectedObject);
        }

        Component* component = dynamic_cast<Component*>(selectedObject);
        Description* description = dynamic_cast<Description*>(selectedObject);

        if (nullptr != component)
        {
            deleteComponent(component);
        }
        else if (nullptr != description)
        {
            deleteDescription(description);
        }
    }
}

void NetworkGraphics::deleteSelectedConnections()
{
    for (Connection* selectedConnection : getSelectedConnections())
    {
        deleteConnection(selectedConnection);
    }
}

QList<Connection*> NetworkGraphics::getSelectedConnections()
{
    QList<Connection*> selectedConnections;
    for (Connection* connection : _connections)
    {
        if (connection->isSelected())
        {
            selectedConnections.append(connection);
        }
    }
    return selectedConnections;
}

void NetworkGraphics::moveObjects(QList<GridObject*> objectsToMove, GridObject* objectToMove, int diffX, int diffY)
{
    bool terminationCondition = false;
    QList<GridObject*> notMovedObjects;
    for (GridObject* gridObject : objectsToMove)
    {
        if (gridObject != objectToMove)
        {
            int xPosition = gridObject->getPosition().x();
            int yPosition = gridObject->getPosition().y();

            QPointF* newPosition = new QPointF(xPosition + diffX,
                                               yPosition + diffY);
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
        moveObjects(notMovedObjects, objectToMove, diffX, diffY);
    }
}

void NetworkGraphics::duplicateGridObjects(QList<GridObject*> objectsToDuplicate)
{
    for (GridObject* gridObject : objectsToDuplicate)
    {
        int xPosition = gridObject->getPosition().x();
        int yPosition = gridObject->getPosition().y();

        int xWayToTheRight = Defines::gridLength;
        if (lookingForFreeSpaceToDuplicate(xPosition, yPosition, xWayToTheRight))
        {
            duplicateGridObject(gridObject, xPosition + xWayToTheRight,
                                yPosition);
        }
    }
}

void NetworkGraphics::pasteGridObjects(QList<GridObject*> objectsToPaste, QPointF positionToPaste)
{
    GridObject* firstGridObject = nullptr;
    if (!objectsToPaste.empty())
    {
        firstGridObject = objectsToPaste.first();
    }

    int xSpace = 0;
    int ySpace = 0;
    int i = 1;
    for (GridObject* gridObject : objectsToPaste)
    {
        QPointF scenePosition(positionToPaste.x() - xSpace, positionToPaste.y() - ySpace);
        if (!hasObjectAtPosition(scenePosition))
        {
            duplicateGridObject(gridObject, scenePosition.x(), scenePosition.y());
            calculateDistanceToNextObject(i, firstGridObject, xSpace, ySpace, objectsToPaste);
        }
    }
}

void NetworkGraphics::calculateDistanceToNextObject(int &i, GridObject* firstGridObject, int &xSpace, int &ySpace, QList<GridObject*> objects)
{
    if (i < objects.count() && firstGridObject != nullptr)
    {
        xSpace = firstGridObject->getPosition().x() - objects[i]->getPosition().x();
        ySpace = firstGridObject->getPosition().y() - objects[i]->getPosition().y();
    }
    else if (i == objects.count())
    {
        xSpace = 0;
        ySpace = 0;
    }
    i++;
}
