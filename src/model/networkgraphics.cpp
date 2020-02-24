#include "model/networkgraphics.h"


NetworkGraphics::NetworkGraphics() : QGraphicsScene(), _graphics(new QGraphicsScene())
{
    // die Anfangsgröße wird initialisiert
    setSceneRect(-_defaultSceneSize, -_defaultSceneSize, _defaultSceneSize, _defaultSceneSize);
    _manager = new FileManager(this);
}

/*!
* \brief Fügt eine Verbindung dem Netzwerk hinzu.
*
* \param[in]    componentPortA  Port der Komponente 1.
* \param[in]    componentPortB  Port der Komponente 2.
*
* Zuerst wird geprüft, ob die Verbindung bereits der connectionList hinzugefügt wurde.
* Ist noch keine Verbindung vorhanden, wird diese hinzugefügt.
*/
void NetworkGraphics::addConnection(ComponentPort componentPortA, ComponentPort componentPortB)
{
    Connection* connection = new Connection(componentPortA, componentPortB, this);
    bool isAlreadyExisting = false;
    for (Connection* otherConnection : _connectionList)
    {
        if (*otherConnection == *connection)
        {
            isAlreadyExisting = true;
            break;
        }
    }
    if (!isAlreadyExisting)
    {
        _connectionList.append(connection);
        addItem(connection);
        update();
    }

    if (!_isLoading)
    {
        updateCalc();
    }
}


void NetworkGraphics::addObject(Component* component)
{
    _componentList.append(component);
    connectComponentToNeighbours(component);
    addItem(component);

    update();

    if (!_isLoading)
    {
        updateCalc();
    }
}

/*!
* \brief Gibt ein Component aus der ComponentList aus, welches an den Soll-Koordinaten x und y ist.
*
* \param[in]    equalX Vergleicht die Soll-X-Koordinate mit einem Component aus der Liste.
* \param[in]    equalY Vergleicht die Soll-Y-Koordinate mit einem Component aus der Liste.
* \return
*
* Es werden nacheinander Components aus der Liste genommen und verglichen, ob ihre Koordinaten mit den Soll-Koordinaten übereinstimmen.
* Wenn ein Component mit den Soll-Koordinaten gefunden wurde, wird dieses zurückgegeben, ansonsten wird der Nullpointer zurückgegeben.
*/
Component* NetworkGraphics::getComponentAtPosition(QPointF gridPosition)
{
    for (Component* component : _componentList)
    {
        bool equalX = (component->getXPosition() == gridPosition.toPoint().x());
        bool equalY = (component->getYPosition() == gridPosition.toPoint().y());
        if (equalX && equalY)
        {
            return component;
        }
    }
    return nullptr;
}

/*!
* \brief Gibt eine Describtion aus der Liste aus Descriptions aus, welches an den Soll-Koordinaten x und y ist.
*
* \param[in]    equalX Vergleicht die Soll-X-Koordinate mit einer Description aus der Liste
* \param[in]    equalY Vergleicht die Soll-Y-Koordinate mit einer Description aus der Liste
* \return
*
* Es werden nacheinander Description aus der Liste genommen und verglichen, ob ihre Koordinaten mit den Soll-Koordinaten übereinstimmen.
* Wenn ein Component mit den Soll-Koordinaten gefunden wurde, wird dieses zurückgegeben, ansonsten wird der Nullpointer zurückgegeben.
*/
DescriptionField* NetworkGraphics::getDescriptionAtPosition(QPointF gridPosition)
{
    for (DescriptionField* description : _descriptions)
    {
        bool equalX = (description->getXPos() == gridPosition.toPoint().x());
        bool equalY = (description->getYPos() == gridPosition.toPoint().y());
        if (equalX && equalY)
        {
            return description;
        }
    }
    return nullptr;
}

/*!
* \brief Prüft ob sich an einer bestimmten Koordinate, eine Komponente oder Textfeld befindet.
*
* \param[in]    gridPosition    ist die zu überprüfende GitterPositon
* \return Gibt zurück, ob sich an der Gitterposition eine Komponente oder Textfeld befindet.
* 
*/
bool NetworkGraphics::isThereAComponentOrADescription(QPointF gridPosition)
{
    return getComponentAtPosition(gridPosition) != nullptr || getDescriptionAtPosition(gridPosition) != nullptr;
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
            if (equalX && equalY)
            {
                return connection;
            }
        }
    }
    return nullptr;
}

QString NetworkGraphics::getFileName()
{
    return _manager->getFileName();
}

/*!
* \brief Gibt einen Componentport aus der ComponentList aus, welcher sich an den Soll-Position in der Scene befinden.
*
* \param[in]    hasFoundPort    gibt an, ob es in der ComponentList ein Component gibt, der an der Sceneposition einen Port hat .
* \param[in]    port            kriegt vom gefundenem Component einen Port zugewiesen.
* \param[in]    cp              ist der Componentport der an der Sceneposiotion gefunden wurde und auch zurückgegeben wird.
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
        if (hasFoundPort)
        {
            Component::Port port = component->getPort(scenePosition);
            ComponentPort* cp = new ComponentPort(component, port);
            return cp;
        }
    }
    return nullptr;
}

/*!
* \brief Gibt den Widerstandswert aus.
*
* \return   Gibt den Widerstandswert als double zurück
*
* Die Methode sucht, ob ein neuer Gesamtwiderstandswert verfügbar ist.
* Anschließend wird der Wert ausgegeben.
*/
double NetworkGraphics::calculate(void)
{
    updateCalc();
    return _calculator.getResistanceValue();
}

void NetworkGraphics::save(void)
{
    _manager->save();

}

void NetworkGraphics::load(void)
{
    _isLoading = true;
    _manager->load();
    _isLoading = false;

    updateCalc();
    emit resistanceValueChanged();
    if(_componentList.count() != 0)
    {
        qDebug() << "List != 0";
        emit newNetworkIsLoad();
    }
}

void NetworkGraphics::saveAs(void)
{
    _manager->saveAs();
}

/*!
* \brief
*
* \param[in]    component  Port der Komponente 1
*
*/
void NetworkGraphics::mirrorComponent(Component* component)
{
    for (Connection* connection : _connectionList)
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
* \param[in]    gridPosition
* \param[in]    componentType
* \param[in]    componentIsVertical
* \return Gibt eine neue Komponente zurück.
*
* Zu Beginn wird geprüft ob sich an der ausgewählten Position bereits eine Komponente befindet.
* Anschließend wird anhand des ausgewählten Typs, jeweils ein Widerstand oder eine Spannungsquelle erzeugt.
*/
Component* NetworkGraphics::createNewComponent(QPointF gridPosition,
                                               Component::ComponentType componentType, bool componentIsVertical)
{
    Component* createdComponent = nullptr;

    if (isThereAComponentOrADescription(gridPosition))
    {
        return nullptr;
    }

    if (Component::ComponentType::Resistor == componentType)
    {

        createdComponent = addResistor("", 100, gridPosition.x(), gridPosition.y(), componentIsVertical);
    }
    else if (Component::ComponentType::PowerSupply == componentType)
    {
        createdComponent = addPowerSupply("", gridPosition.x(), gridPosition.y(), componentIsVertical);
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
* \param[in]    componentToDuplicate ist die zu duplizierende Komponente
* \param[in]    xPosition ist die X-Koordinate der übergebenden Komponente
* \param[in]    yPosition ist die Y-Koordinate der übergebenden Komponente
* \return Gibt die duplizierte Komponente zurück.
*
* Zuerst wird der Name, Wert und die räumliche Ausrichtung der zu kopierenden Komponente erfragt.
* Anschließend wird abhängig vom Typ der Komponente ein Widerstand oder ein Spannungsquelle erzeugt.
* Dann wird der Gesamtwiderstand aktualisiert.
*/
Component* NetworkGraphics::duplicateComponent(Component* componentToDuplicate, int xPosition, int yPosition)
{
    Component* duplicatedComponent = nullptr;

    QString name = componentToDuplicate->getName();
    int value = componentToDuplicate->getValue();
    bool componentIsVertical = componentToDuplicate->isVertical();

    if (Component::ComponentType::Resistor == componentToDuplicate->getComponentType())
    {
        duplicatedComponent = addResistor(name, value, xPosition, yPosition, componentIsVertical);
    }
    else if (Component::ComponentType::PowerSupply == componentToDuplicate->getComponentType())
    {
        duplicatedComponent = addPowerSupply(name, xPosition, yPosition, componentIsVertical);
    }

    updateCalc();
    return duplicatedComponent;
}

/*!
* \brief Verdoppelt ein Textfeld.
*
* \param[in]    descriptionToDuplicate  ist das zu verdoppelde Textfeld
* \param[in]    xPosition               ist die X-Koordinate, an der das Textfeld erzeugt werden soll
* \param[in]    yPosition               ist die Y-Koordinate, an der das Textfeld erzeugt werden soll
* \return Gibt ein neues Textfeld zurück.
*/
DescriptionField*
NetworkGraphics::duplicateDescription(DescriptionField* descriptionToDuplicate, int xPosition, int yPosition)
{
    return createDescriptionField(QPointF(xPosition, yPosition), false, descriptionToDuplicate->getText());
}


/*!
* \brief Fügt einen Widerstand dem Netzwerk hinzu.
*
* \param[in]    name            ist der Name des Widerstandsobjektes
* \param[in]    valueResistance ist der zugewiesene Widerstandswertes
* \param[in]    x               ist die X-Koordinate der zugewiesenen Position
* \param[in]    y               ist die Y-Koordinate der zugewiesenen Position
* \param[in]    isVertical      ist die räumliche Ausrichtung im Netzwerk
* \param[in]    id              ist die intern zugewiesene Id des Widerstandes
* \return Gibt einen neuen Widerstand zurück.
*
* Falls noch kein Name vorhanden ist, wird dieser neu erzeugt.
* Anschließend wird ein neuer Widerstand erzeugt.
* Es wird nach dem hinzufügen der Widerstandswert neu berechnet.
*/

Component*
NetworkGraphics::addResistor(QString name, int valueResistance, int xPosition, int yPosition, bool isVertical, int id)
{
    _resistorCount++;
    if ("" == name)
    {
        name = "R" + QString::number(_resistorCount);
    }
    if (0 == id)
    {
        id = _resistorCount;
    }

    Component* resistor = new Resistor(name, valueResistance, xPosition, yPosition,
                                       isVertical, id);
    addObject(resistor);
    return resistor;
}

/*!
* \brief Fügt eine Spannungsquelle dem Netzwerk hinzu.
*
* \param[inout]    name            ist der Name des Spannungsquellenobjektes
* \param[inout]    x               ist die X-Koordinate der zugewiesenen Position
* \param[inout]    y               ist die Y-Koordinate der zugewiesenen Position
* \param[inout]    isVertical      ist die räumliche Ausrichtung im Netzwerk
* \param[inout]    id              ist die intern zugewiesene Id der Spannungsquelle
* \return Gibt eine neue Spannungsquelle zurück.
*
* Falls noch kein Name vorhanden ist, wird dieser neu erzeugt.
* Befindet sich im Netzwerk bereits eine Spannungsquelle, kann keine weitere erzeugt werden.
* Anschließend wird eine neue Spannungsquelle erzeugt.
*/
Component* NetworkGraphics::addPowerSupply(QString name, int x, int y, bool isVertical, int id)
{
    _powerSupplyCount++;
    if ("" == name)
    {
        name = "Q" + QString::number(_powerSupplyCount);
    }

    if (_powerSupplyCount <= 1)
    {
        Component* powerSupply = new PowerSupply(name, x, y,
                                                 isVertical, id);
        addObject(powerSupply);
        return powerSupply;
    }
    else
    {
        _powerSupplyCount--;
        QMessageBox::about(nullptr, "Fehleingabe", "Nur eine Spannungsquelle erlaubt");
    }
    return nullptr;
}

/*!
* \brief Erzeugt ein neues Textfeld.
*
* \param[inout]     gridPosition    ist die Gitterposition innerhalb im Netzwerk
* \param[in]        isLoad          ist ein bool der angibt, ob das Textfeld bereits geladen wurde
* \param[inout]     text            ist der Text des Textfeldes
* \param[inout]     id              ist die Id des Textfeldes
* \return Gibt ein neues Textfeld zurück.
*
* Es wird zuerst geprüft ob das Textfeld bereits geladen wurde.
* Wenn das Textfeld noch nicht geladen wurde, wird geprüft, ob sich an der Gitterposition bereits eine Komponente oder ein Textfeld befindet.
* Dann wird die Id des Textfeldes gesetzt.
* Wenn das Textfeld bereits geladen wurde entfallen die beiden oberen Schritte.
* Anschließend wird ein neues Textfeld erzeugt und der descriptionCount um eins erhöht.
*/
DescriptionField*
NetworkGraphics::createDescriptionField(QPointF gridPosition, bool isLoad, [[maybe_unused]] QString text,
                                        [[maybe_unused]] int id)
{
    if (!isLoad)
    {
        if (isThereAComponentOrADescription(gridPosition))
        {
            return nullptr;
        }
        id = _descriptionCount;
    }

    DescriptionField* description = new DescriptionField(gridPosition.x(), gridPosition.y(), id, text);

    _descriptions.append(description);
    addItem(description);

    update();

    _descriptionCount++;
    return description;
}

/*!
* \brief Entfernt Komponenten aus dem Netzwerk.
*
* \param[in]    component ist die zu entfernende Komponente
*
* Wenn die ausgewählte Komponente vorhanden ist, wird diese entfernt.
* Nach dem entfernen aus dem Netzwerk wird die Komponente aus dem componentList entfernt.
* Wenn es sich bei der Komponente um einen Widerstand handelt wird der resistorCount um 1 reduziert.
* Wenn es sich um eine Spannungsquellle handelt geschieht dies analog zum Widerstand.
* Anschließend werden die Verbindungen, die sich an der Komponente befinden entfernt.
* Zum Schluss wird der Widerstandswert neu berechnet.
*/
void NetworkGraphics::deleteComponent(Component* component)
{
    if (component != nullptr)
    {
        removeItem(component);
        _componentList.removeOne(component);

        //ResistorCount und PowerSupplyCount setzen
        if (Component::ComponentType::Resistor == component->getComponentType())
        {
            _resistorCount--;
        }
        else if (Component::ComponentType::PowerSupply == component->getComponentType())
        {
            _powerSupplyCount--;
        }

        //Lösche Verbindungen vom Component
        for (Connection* connection : _connectionList)
        {
            if (connection->hasComponent(component))
            {
                removeItem(connection);
                _connectionList.removeOne(connection);
                delete connection;
            }
        }

        updateCalc();
        delete component;
    }
    update();
}

/*!
* \brief Entfernt ein Textfeld aus dem Netzwerk.
*
* \param[in]    description ist das zu entfernende Textfeld
*
* Wenn das ausgewählte Textfeld vorhanden ist, wird diese entfernt.
* Dannach wird das Textfeld aus der Liste descriptions entfernt.
* Der descriptionCount wird um eins reduziert.
*/
void NetworkGraphics::deleteDescription(DescriptionField* description)
{
    if (description != nullptr)
    {
        removeItem(description);
        _descriptions.removeOne(description);
        _descriptionCount--;

        delete description;
    }
}

/*!
* \brief Entfernt eine Verbindung aus dem Netzwerk.
*
* \param[in]    connecton ist die zu entfernende Verbindung
*
* Wenn die ausgewählte Verbindung vorhanden ist, wird diese entfernt.
* Nach dem entfernen aus dem Netzwerk wird die Verbindung aus der connectionList entfernt.
*/
void NetworkGraphics::deleteConnection(Connection* connection)
{
    if (connection != nullptr)
    {
        removeItem(connection);
        _connectionList.removeOne(connection);
        delete connection;
    }
}

/*!
* \brief Verschiebt eine Komponente im Netzwerk.
*
* \param[in]    componentToMove ist die zu verschiebende Komponente
* \param[in]    descriptionToMove ist die zu verschiebende Beschreibung
* \param[in]    gridPosition ist die Position, an die die Komponente verschoben werden soll
*
* Es wird zuerst geprüft ob sich an der neuen Gitterposition bereits eine Komponente oder ein Textfeld befindet.
* Befindet sich an der Position nichts, wird die ausgewählte Komponente an die neu Position verschoben.
*/
void
NetworkGraphics::moveComponent(Component* componentToMove, DescriptionField* descriptionToMove, QPointF gridPosition)
{
    bool isComponentAtPosition = isThereAComponentOrADescription(gridPosition);
    if (isComponentAtPosition)
    {
        return;
    }
    if (componentToMove != nullptr)
    {
        componentToMove->setPosition(gridPosition);
        //Beim Bewegen kann ein Component direkt mit seinen Nachbarn verbunden werden
        connectComponentToNeighbours(componentToMove);
    }
    else if (descriptionToMove != nullptr)
    {
        descriptionToMove->setPosition(gridPosition);
    }
}

void NetworkGraphics::connectComponentToNeighbours(Component* componentToConnectWithNeighbours)
{
    QPointF portPositionAOfComponent = componentToConnectWithNeighbours->getPortPosition(Component::Port::A);
    QPointF portPositionBOfComponent = componentToConnectWithNeighbours->getPortPosition(Component::Port::B);

    for (Component* anotherComponent : _componentList)
    {
        if (anotherComponent != componentToConnectWithNeighbours)
        {
            QPointF portPositionAOfanotherComponent = anotherComponent->getPortPosition(Component::Port::A);
            QPointF portPositionBOfanotherComponent = anotherComponent->getPortPosition(Component::Port::B);

            bool equalPortAtoA = (portPositionAOfComponent == portPositionAOfanotherComponent);
            bool equalPortAtoB = (portPositionAOfComponent == portPositionBOfanotherComponent);
            bool equalPortBtoA = (portPositionBOfComponent == portPositionAOfanotherComponent);
            bool equalPortBtoB = (portPositionBOfComponent == portPositionBOfanotherComponent);

            ComponentPort componentPortSelf(nullptr, Component::Port::null);
            ComponentPort componentPortOther(nullptr, Component::Port::null);

            if (equalPortAtoA)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::A);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::A);
            }
            else if (equalPortAtoB)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::A);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::B);
            }
            else if (equalPortBtoA)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::B);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::A);
            }
            else if (equalPortBtoB)
            {
                componentPortSelf = ComponentPort(componentToConnectWithNeighbours, Component::Port::B);
                componentPortOther = ComponentPort(anotherComponent, Component::Port::B);
            }

            bool foundEqualPort = (equalPortAtoA || equalPortAtoB || equalPortBtoA || equalPortBtoB);
            if (foundEqualPort)
            {
                addConnection(componentPortSelf, componentPortOther);
            }
        }
    }
}

/*!
* \brief Dreht die Komponente linksrum.
*
* \param[in]    componentToTurn ist die zu drehende Komponente
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
* \param[in]    componentToTurn ist die zu drehende Komponente
*
*/
void NetworkGraphics::turnComponentRight(Component* componentToTurn)
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
}

/*!
* \brief Setzt die Orientierung einer ausgewählten Komponente.
*
* \param[in]    componentToTurn ist die zu drehende Komponente
* \param[in]    orientation     ist die Orientierung der zu drehenden Komponente
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
*/
void NetworkGraphics::updateCalc(void)
{
    _calculator.setLists(_connectionList, _componentList);
    _calculator.calculate();

    _puzzleCalculator.setLists(_connectionList, _componentList);
    //_puzzleCalculator.calculate();

    emit resistanceValueChanged();
}
