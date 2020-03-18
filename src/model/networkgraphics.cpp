#include "defines.h"
#include "model/networkgraphics.h"
#include "model/calculator.h"

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
    for (Connection* otherConnection : _connectionList)
    {
        if(*otherConnection == *connection)
        {
            isAlreadyExisting = true;
            createdConnection = otherConnection;
            break;
        }
    }
    if(!isAlreadyExisting)
    {
        _connectionList.append(connection);
        createdConnection = connection;
        addItem(connection);
        update();
    }
    else
    {
        delete connection;
    }

    if(!_isLoading)
    {
        updateCalc();
    }

    return createdConnection;
}


void NetworkGraphics::addObject(Component* component)
{
    _componentList.append(component);
    //Hier kann der Widerstand direkt mit den Nachbarn bzw. angrenzenden Widerständen verbunden werden
    //connectComponentToNeighbours(component);
    addItem(component);

    updateCalc();
}

/*!
 * \brief Gibt ein Component aus der ComponentList aus, welches an den Soll-Koordinaten x und y ist.
 *
 * \param   scenePosition   ist die zu überprüfende Gitterposition
 * \return Gibt die Komponente an der gesuchten Position zurück.
 *
 * Es werden nacheinander Components aus der Liste genommen und verglichen, ob ihre Koordinaten mit den Soll-Koordinaten übereinstimmen.
 * Wenn ein Component mit den Soll-Koordinaten gefunden wurde, wird dieses zurückgegeben, ansonsten wird der Nullpointer zurückgegeben.
 */
Component* NetworkGraphics::getComponentAtPosition(QPointF scenePosition)
{
    QPointF gridPosition = mapSceneToGrid(scenePosition);
    for (Component* component : _componentList)
    {
        bool equalX = (component->getXPosition() == gridPosition.toPoint().x());
        bool equalY = (component->getYPosition() == gridPosition.toPoint().y());
        if(equalX && equalY)
        {
            return component;
        }
    }
    return nullptr;
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
DescriptionField* NetworkGraphics::getDescriptionAtPosition(QPointF scenePosition)
{
    QPointF gridPosition = mapSceneToGrid(scenePosition);
    for (DescriptionField* description : _descriptionList)
    {
        bool equalX = (description->getXPosition() == gridPosition.toPoint().x());
        bool equalY = (description->getYPosition() == gridPosition.toPoint().y());
        if(equalX && equalY)
        {
            return description;
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
    return getComponentAtPosition(scenePosition) != nullptr || getDescriptionAtPosition(scenePosition) != nullptr;
}

Connection* NetworkGraphics::getConnectionAtPosition(QPointF gridposition)
{
    for (Connection* connection : _connectionList)
    {
        for (QRect* hitBox : connection->getHitboxList())
        {
            bool equalX = ((gridposition.toPoint().x() >= hitBox->x()) &&
                           (gridposition.toPoint().x() <= (hitBox->x() + hitBox->width())));
            bool equalY = ((gridposition.toPoint().y() >= hitBox->y()) &&
                           (gridposition.toPoint().y() <= (hitBox->y() + hitBox->height())));
            if(equalX && equalY)
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
    for (Component* component : _componentList)
    {
        bool hasFoundPort = component->hasPortAtPosition(scenePosition);
        if(hasFoundPort)
        {
            Component::Port port = component->getPort(scenePosition);
            ComponentPort* cp = new ComponentPort(component, port);
            return cp;
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
    if(_componentList.count() != 0)
    {
        bool powerSupplyFound = false;
        for (Component* component : _componentList)
        {
            if(component->getComponentType() == Component::PowerSupply)
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
    for (Connection* connection : _connectionList)
    {
        if(connection->hasComponent(component))
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

    if(hasObjectAtPosition(gridPosition))
    {
        return nullptr;
    }

    if(Component::ComponentType::Resistor == componentType)
    {

        createdComponent = addResistor("", 100, gridPosition.x(), gridPosition.y(), componentIsVertical);
    }
    else if(Component::ComponentType::PowerSupply == componentType)
    {
        createdComponent = addPowerSupply("", gridPosition.x(), gridPosition.y(), componentIsVertical, 100, 0);

        emit powerSupplyIsAllowed(false);

    }

    if(!_isLoading)
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
    if(isResistor)
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
DescriptionField*
NetworkGraphics::duplicateDescription(DescriptionField* descriptionToDuplicate, int xPosition, int yPosition)
{
    CommandDuplicateDescription* commandDuplicateDescription = new CommandDuplicateDescription(this,
                                                                                               descriptionToDuplicate,
                                                                                               xPosition, yPosition);
    _undoStack->push(commandDuplicateDescription);
    //return addDescriptionFieldWithoutUndo(QPointF(xPosition, yPosition), false, descriptionToDuplicate->getText());
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
    _resistorCount++;
    if("" == name)
    {
        name = "R" + QString::number(_resistorCount);
    }
    if(0 == id)
    {
        //Finden einer freien ID
        int newId = _resistorCount;
        bool isSetId = false;
        while (!isSetId)
        {
            bool isIdValid = true;
            for (Component* component : _componentList)
            {
                if(component->getId() == newId)
                {
                    isIdValid = false;
                }
            }
            if(isIdValid)
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
    addObject(resistor);
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
    if("" == name)
    {
        name = "Q" + QString::number(_powerSupplyCount);
    }

    if(_powerSupplyCount < 1)
    {
        _powerSupplyCount++;
        Component* powerSupply = new PowerSupply(name, x, y,
                                                 isVertical, voltage, id);
        addObject(powerSupply);
        emit powerSupplyIsAllowed(false);
        return powerSupply;
    }
    else
    {
        QMessageBox::about(nullptr, "Fehleingabe", "Nur eine Spannungsquelle erlaubt");
    }
    return nullptr;
}

/*!
 * \brief Schneidet ausgewählte Komponenten aus.
 *
 * \param componentToCut ist die Komponente die Ausgeschnitten werden soll
 */
void NetworkGraphics::cutComponentWithoutUndo(Component* componentToCut)
{
    if(nullptr != componentToCut)
    {
        removeItem(componentToCut);
        _componentList.removeOne(componentToCut);
        if(componentToCut->getComponentType() == Component::PowerSupply)
        {
            emit powerSupplyIsAllowed(true);
            _powerSupplyCount--;
        }
        update();
    }
}

void NetworkGraphics::cutDescriptionWithoutUndo(DescriptionField* descriptionToCut)
{
    if (nullptr != descriptionToCut)
    {
        removeItem(descriptionToCut);
        _descriptionList.removeOne(descriptionToCut);
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
DescriptionField*
NetworkGraphics::addDescriptionFieldWithoutUndo(QPointF gridPosition, bool isLoad, [[maybe_unused]] QString text,
                                                [[maybe_unused]] int id)
{
    if(!isLoad)
    {
        if(hasObjectAtPosition(gridPosition))
        {
            return nullptr;
        }
        id = _descriptionCount;
    }

    DescriptionField* description = new DescriptionField(gridPosition.x(), gridPosition.y(), id, text);
    addDescriptionFieldWithoutUndo(description);

    return description;
}

/*!
 * \brief Fügt ein Textfeld dem Netzwerk hinzu.
 *
 * \param   descriptionFieldToAdd   ist das Textfeld, welches hinzugefügt wird
 *
 */
void NetworkGraphics::addDescriptionFieldWithoutUndo(DescriptionField* descriptionFieldToAdd)
{
    _descriptionList.append(descriptionFieldToAdd);
    addItem(descriptionFieldToAdd);

    update();

    _descriptionCount++;
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
    if(component != nullptr)
    {
        removeItem(component);
        _componentList.removeOne(component);

        //ResistorCount und PowerSupplyCount setzen
        if(Component::ComponentType::Resistor == component->getComponentTypeInt())
        {
            _resistorCount--;
        }
        else if(Component::ComponentType::PowerSupply == component->getComponentTypeInt())
        {
            _powerSupplyCount--;
            emit powerSupplyIsAllowed(true);
        }

        //Lösche Verbindungen vom Component
        for (Connection* connection : _connectionList)
        {
            if(connection->hasComponent(component))
            {
                removeItem(connection);
                _connectionList.removeOne(connection);
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
void NetworkGraphics::deleteDescriptionWithoutUndo(DescriptionField* description)
{
    if(description != nullptr)
    {
        removeItem(description);
        _descriptionList.removeOne(description);
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
    if(connection != nullptr)
    {
        removeItem(connection);
        _connectionList.removeOne(connection);
    }

    updateCalc();
    update();
}

/*!
 * \brief Verschiebt eine Komponente im Netzwerk.
 *
 * \param   componentToMove    ist die zu verschiebende Komponente
 * \param   descriptionToMove  ist die zu verschiebende Beschreibung
 * \param   gridPosition       ist die Position, an die die Komponente verschoben werden soll
 *
 * Es wird zuerst geprüft ob sich an der neuen Gitterposition bereits eine Komponente oder ein Textfeld befindet.
 * Befindet sich an der Position nichts, wird die ausgewählte Komponente an die neu Position verschoben.
 */
void
NetworkGraphics::moveComponentWithoutUndo(Component* componentToMove, DescriptionField* descriptionToMove,
                                          QPointF gridPosition)
{
    bool isComponentAtPosition = hasObjectAtPosition(gridPosition);
    if(isComponentAtPosition)
    {
        return;
    }
    if(componentToMove != nullptr)
    {
        componentToMove->setPosition(gridPosition);
    }
    else if(descriptionToMove != nullptr)
    {
        descriptionToMove->setPosition(gridPosition);
    }
    update();
}

void NetworkGraphics::connectComponentToNeighbours(Component* componentToConnectWithNeighbours)
{
    QPointF portPositionAOfComponent = componentToConnectWithNeighbours->getPortPosition(Component::Port::A);
    QPointF portPositionBOfComponent = componentToConnectWithNeighbours->getPortPosition(Component::Port::B);

    for (Component* anotherComponent : _componentList)
    {
        if(anotherComponent != componentToConnectWithNeighbours)
        {
            QPointF portPositionAOfanotherComponent = anotherComponent->getPortPosition(Component::Port::A);
            QPointF portPositionBOfanotherComponent = anotherComponent->getPortPosition(Component::Port::B);

            bool equalPortAtoA = (portPositionAOfComponent == portPositionAOfanotherComponent);
            bool equalPortAtoB = (portPositionAOfComponent == portPositionBOfanotherComponent);
            bool equalPortBtoA = (portPositionBOfComponent == portPositionAOfanotherComponent);
            bool equalPortBtoB = (portPositionBOfComponent == portPositionBOfanotherComponent);

            ComponentPort componentPortSelf(nullptr, Component::Port::null);
            ComponentPort componentPortOther(nullptr, Component::Port::null);

            if(equalPortAtoA)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::A);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::A);
            }
            else if(equalPortAtoB)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::A);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::B);
            }
            else if(equalPortBtoA)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::B);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::A);
            }
            else if(equalPortBtoB)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::B);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::B);
            }

            bool foundEqualPort = (equalPortAtoA || equalPortAtoB || equalPortBtoA || equalPortBtoB);
            if(foundEqualPort)
            {
                addConnection(componentPortSelf, componentPortOther);
            }
        }
    }
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
 * Die aktuellen _connectionList und _componentList werden dem Calculator übergeben.
 * Anschließend wird der aktualisierte Widerstandswert ausgegeben.
 */
void NetworkGraphics::updateCalc(void)
{
    if(!_isLoading)
    {
        _resistanceValue = Calculator::calculator().calculate(_connectionList, _componentList);

        update();
        emit resistanceValueChanged();
        if(_resistanceValue != 0)
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
 * \param   componentToMove     ist die zu verschiebene Komponente
 * \param   descriptionToMove   ist das zu verschieben Textfeld
 * \param   gridPosition        neue Position
 *
 * Wenn eine Komponente ausgewählt ist, wird die Komponente verschoben.
 */
void
NetworkGraphics::moveComponent(Component* componentToMove, DescriptionField* descriptionToMove, QPointF gridPosition)
{
    if(componentToMove != nullptr)
    {
        CommandMoveComponent* commandMoveComponent = new CommandMoveComponent(this, componentToMove, descriptionToMove,
                                                                              gridPosition);
        _undoStack->push(commandMoveComponent);
    }
    else if(descriptionToMove != nullptr)
    {
        CommandMoveComponent* commandMoveComponent = new CommandMoveComponent(this, componentToMove, descriptionToMove,
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
    if(isResistor)
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
            //mirrorComponent(componentToTurn);
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
    Component::ComponentType componentType = componentToAdd->getComponentType();

    if(hasObjectAtPosition(gridPosition))
    {
        return;
    }

    if(Component::ComponentType::Resistor == componentType)
    {
        _resistorCount++;
        addObject(componentToAdd);
    }
    else if(Component::ComponentType::PowerSupply == componentType)
    {
        if(_powerSupplyCount < 1)
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
    for (Connection* otherConnection : _connectionList)
    {
        if(*otherConnection == *connection)
        {
            isAlreadyExisting = true;
            break;
        }
    }
    if(!isAlreadyExisting && nullptr != connection)
    {
        _connectionList.append(connection);
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
void NetworkGraphics::deleteDescription(DescriptionField* descriptionFieldToDelete)
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
void NetworkGraphics::editDescription(DescriptionField* descriptionToEdit, QString newText)
{
    CommandEditDescription* commandEditDescription = new CommandEditDescription(this, descriptionToEdit, newText);
    _undoStack->push(commandEditDescription);
}

/*!
 * \brief Verschiebt zusammengefasste Komponente an eine andere Gitterposition.
 *
 * \param componentList     ist die Liste der zusammengefassten Komponenten
 * \param descriptionList   ist die Liste der zusammengefassten Textfelder
 * \param componentToMove   ist die ausgewählte Komponente
 * \param descriptionToMove ist das ausgewählte Textfeld
 * \param diffXAfterMoving  ist die Differenz zwischen der Ausgangs und der End X-Koordinate
 * \param diffYAfterMoving  ist die Differenz zwischen der Ausgangs und der End Y-Koordinate
 */
void
NetworkGraphics::moveMultiselectComponents(QList<Component*> componentList, QList<DescriptionField*> descriptionList,
                                           Component* componentToMove, DescriptionField* descriptionToMove,
                                           int diffXAfterMoving, int diffYAfterMoving)
{
    bool terminationCondition = false;
    QList<Component*> notMovedComponents;
    QList<DescriptionField*> notMovedDescriptions;
    for (Component* component : componentList)
    {
        if(component->isSelected() && component != componentToMove)
        {
            QPointF* newPosition = new QPointF(component->getXPosition() + diffXAfterMoving,
                                               component->getYPosition() + diffYAfterMoving);
            if(hasObjectAtPosition(*newPosition))
            {
                if(getComponentAtPosition(*newPosition) != nullptr &&
                   !getComponentAtPosition(*newPosition)->isSelected())
                {
                    terminationCondition = true;
                }
                if(getDescriptionAtPosition(*newPosition) != nullptr &&
                   !getDescriptionAtPosition(*newPosition)->isSelected())
                {
                    terminationCondition = true;
                }
                notMovedComponents.append(component);
            }
            DescriptionField* placeholder = nullptr;
            moveComponent(component, placeholder, *newPosition);
            delete placeholder;
        }
    }
    for (DescriptionField* descriptionfield : descriptionList)
    {
        if(descriptionfield->isSelected() && descriptionfield != descriptionToMove)
        {
            QPointF* newPosition = new QPointF(descriptionfield->getXPosition() + diffXAfterMoving,
                                               descriptionfield->getYPosition() + diffYAfterMoving);
            if(hasObjectAtPosition(*newPosition))
            {
                if(getComponentAtPosition(*newPosition) != nullptr &&
                   !getComponentAtPosition(*newPosition)->isSelected())
                {
                    terminationCondition = true;
                }
                if(getDescriptionAtPosition(*newPosition) != nullptr &&
                   !getDescriptionAtPosition(*newPosition)->isSelected())
                {
                    terminationCondition = true;
                }
                notMovedDescriptions.append(descriptionfield);
            }
            Resistor* placeholder = nullptr;
            moveComponent(placeholder, descriptionfield, *newPosition);
            delete placeholder;
        }
    }
    if(terminationCondition)
    {
        return;
    }
    if(!notMovedComponents.isEmpty() || !notMovedDescriptions.isEmpty())
    {
        moveMultiselectComponents(notMovedComponents, notMovedDescriptions, componentToMove, descriptionToMove,
                                  diffXAfterMoving, diffYAfterMoving);
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

void NetworkGraphics::cutDescription(DescriptionField* descriptionToCut)
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
DescriptionField* NetworkGraphics::addDescriptionField(QPointF gridPosition, bool isLoad, QString text, int id)
{
    if(!isLoad)
    {
        if(hasObjectAtPosition(gridPosition))
        {
            return nullptr;
        }
        id = _descriptionCount;
    }

    DescriptionField* description = new DescriptionField(gridPosition.x(), gridPosition.y(), id, text);

    CommandAddDescriptionField* commandAddDescriptionField = new CommandAddDescriptionField(this, description);
    _undoStack->push(commandAddDescriptionField);

    return description;
}

QString NetworkGraphics::getVoltageAndCurrentInformation(void)
{
    QString information;
    if(!Calculator::calculator().hasUsedStarCalculation())
    {
        for (Component* c : getComponents())
        {
            if(c->getComponentType() == Component::PowerSupply)
            {
                information +=
                        "Gesamtspannung: " + QString::number(c->getVoltage(), 'f', 2) + "V" + "<br>" + "Gesamtstrom: " +
                        QString::number(c->getAmp(), 'f', 2)
                        + "A" + "<br> <br>";
            }
        }
        for (Component* c : getComponents())
        {
            if(c->getComponentType() == Component::Resistor)
            {
                Resistor* resistor = dynamic_cast<Resistor*>(c);
                bool isResistor = (nullptr != resistor);
                if(isResistor)
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

QList<Component*> NetworkGraphics::getSelectedComponents(void)
{
    QList<Component*> selectedComponents;
    for (Component* component : _componentList)
    {
        if(component->isSelected())
        {
            selectedComponents.append(component);
        }
    }
    return selectedComponents;
}

QList<DescriptionField*> NetworkGraphics::getSelectedDescriptionFields(void)
{
    QList<DescriptionField*> selectedDescriptionFields;
    for (DescriptionField* description : _descriptionList)
    {
        if(description->isSelected())
        {
            selectedDescriptionFields.append(description);
        }
    }
    return selectedDescriptionFields;
}

void NetworkGraphics::deselectAllItems(void)
{
    for (Component* component : getComponents())
    {
        component->setIsSelected(false);
    }

    for (DescriptionField* descriptionfield : getDescriptions())
    {
        descriptionfield->setIsSelected(false);
    }

    for (Connection* connection : getConnections())
    {
        connection->setIsSelected(false);
    }

    update();
}

void NetworkGraphics::selectObjectsAtPosition(QPointF scenePosition)
{
    Component* foundComponent = getComponentAtPosition(scenePosition);
    DescriptionField* foundDescription = getDescriptionAtPosition(scenePosition);
    Connection* foundConnection = getConnectionAtPosition(scenePosition);

    bool hasFoundComponent = nullptr != foundComponent;
    bool hasFoundDescription = nullptr != foundDescription;
    bool hasFoundConnection = nullptr != foundConnection;

    if(hasFoundComponent)
    {
        foundComponent->setIsSelected(true);
    }
    else if(hasFoundDescription)
    {
        foundDescription->setIsSelected(true);
    }
    else if(hasFoundConnection)
    {
        foundConnection->setIsSelected(true);
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
        if(component->isSelected())
        {
            componentList.append(component);
        }
    }
    return componentList;
}

QList<DescriptionField*> NetworkGraphics::findSelectedDescription(void)
{
    QList<DescriptionField*> descriptionList;
    for (DescriptionField* description : getDescriptions())
    {
        if(description->isSelected())
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

void NetworkGraphics::cutDescription(QList<DescriptionField*> descriptions)
{
    for (DescriptionField* descriptionToCut : descriptions)
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
