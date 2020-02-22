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
* \param[in]    componentPortA  Port der Komponente 1
* \param[in]    componentPortB  Port der Komponente 2
*
* Zuerst wird geprüft, ob die Verbindung bereits der connectionList hinzugefügt wurde.
* Ist noch keine Verbindung vorhanden, wird diese hinzugefügt.
*/
void NetworkGraphics::addConnection(ComponentPort componentPortA, ComponentPort componentPortB)
{
    Connection* connection = new Connection(componentPortA, componentPortB, _componentList);
    if (!_connectionList.contains(connection))
    {
        _connectionList.append(connection);
        addItem(connection);
        update();
    }
    else
    {
        qDebug() << "NetworkGraphics: Connection bereits vorhanden!";
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

    if(!_isLoading)
    {
        updateCalc();
    }
}

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

DescriptionField *NetworkGraphics::getDescriptionAtPosition(QPointF gridPosition)
{
    for(DescriptionField* description : _descriptions)
    {
        bool equalX = (description->getXPos() == gridPosition.toPoint().x());
        bool equalY = (description->getYPos() == gridPosition.toPoint().y());
        if(equalX && equalY)
        {
            return description;
        }
    }
    return nullptr;
}

bool NetworkGraphics::isThereAComponentOrADescription(QPointF gridPosition)
{
    return getComponentAtPosition(gridPosition) != nullptr || getDescriptionAtPosition(gridPosition) != nullptr;
}

QString NetworkGraphics::getFileName()
{
    return _manager->getFileName();
}


void NetworkGraphics::reloadAll(void)
{
    for (Component* component : _componentList)
    {
        addItem(component);
    }
    for (Connection* connection : _connectionList)
    {
        addItem(connection);
    }
    update();
}

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
* \return   gibt den Widerstandswert als double zurück
*
* Die Methode sucht, ob ein neuer Widerstandswert verfügbar ist.
*/
double NetworkGraphics::calculate(void)
{
    updateCalc();

    QMessageBox::about(nullptr, "Berechnung", "Der Gesamtwiderstand des Netzwerkes beträgt : " +
                       QString::number(_calculator.getResistanceValue()) + "Ω");
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
* \return
*
* Zu Beginn wird geprüft ob sich an der ausgewählten Position bereits eine Komponente befindet.
* Anschließend wird anhand des ausgewählten Typen die jeweils ein Widerstand oder eine Spannungsquelle erzeugt.
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

    if(!_isLoading)
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
*
*
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

DescriptionField *NetworkGraphics::duplicateDescription(DescriptionField *descriptionToDuplicate, int xPosition, int yPosition)
{
    return  createDescriptionField(QPointF(xPosition,yPosition), false, descriptionToDuplicate->getText());
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
* \return
*
* Falls noch kein Name vorhanden ist, wird dieser neu erzeugt.
* Anschließend wird ein neues Widerstandsobjekt erzeugt.
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
* \param[in]    name            ist der Name des Spannungsquellenobjektes
* \param[in]    x               ist die X-Koordinate der zugewiesenen Position
* \param[in]    y               ist die Y-Koordinate der zugewiesenen Position
* \param[in]    isVertical      ist die räumliche Ausrichtung im Netzwerk
* \param[in]    id              ist die intern zugewiesene Id der Spannungsquelle
* \return
*
* Falls noch kein Name vorhanden ist, wird dieser neu erzeugt.
* Befindet sich im Netzwerk bereits eine Spannungsquelle, kann keine weitere erzeugt werden.
* Anschließend wird ein neues Spannungsquellenobjekt erzeugt.
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
* \param[in]    gridPosition    ist die Gitterposition innerhalb im Netzwerk
* \return
*
* Es wird zuerst geprüft ob sich an der Gitterposition bereits eine Komponente oder ein Textfeld befindet.
* Wenn sich nichts an der position befindet, wird ein neues Textfeld erzeugt.
*/
DescriptionField *NetworkGraphics::createDescriptionField(QPointF gridPosition, bool isLoad, [[maybe_unused]] QString text, [[maybe_unused]] int id)
{
    if(!isLoad)
    {
        if(isThereAComponentOrADescription(gridPosition))
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
}

void NetworkGraphics::deleteDescription(DescriptionField *description)
{
    if(description != nullptr)
    {
        removeItem(description);
        _descriptions.removeOne(description);
        _descriptionCount--;

        delete description;
    }
}

/*!
* \brief Verschiebt eine Komponente im Netzwerk.
*
* \param[in]    componentToMove ist die zu verschiebende Komponente
* \param[in]    descriptionToMove ist die zu verschiebende Beschreibung
* \param[in]    gridPosition    ist die Position, an die die Komponente verschoben werden soll
*
* Es wird das zu verschiebende Objekt verschoben.
*
*/
void NetworkGraphics::moveComponent(Component* componentToMove, DescriptionField *descriptionToMove, QPointF gridPosition)
{
    bool isComponentAtPosition = isThereAComponentOrADescription(gridPosition);
    if (isComponentAtPosition)
    {
        return;
    }
    if(componentToMove != nullptr)
    {
        componentToMove->setPosition(gridPosition);
        connectComponentToNeighbours(componentToMove);
    }
    else if(descriptionToMove != nullptr)
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
* Es wird die Methode turnComponentRight drei Mal aufgerufen, da dies einmal linksrum Drehen entspricht.
*
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


void NetworkGraphics::updateCalc(void)
{
    _calculator.setLists(_connectionList, _componentList);
    _calculator.calculate();
    qDebug() << "Neuer Widerstandswert: " << _calculator.getResistanceValue();
    //TODO: neuen Widerstandswert anzeigen
}
