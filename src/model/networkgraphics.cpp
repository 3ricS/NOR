#include "model/networkgraphics.h"


NetworkGraphics::NetworkGraphics() : QGraphicsScene(), _graphics(new QGraphicsScene())
{
    // die Anfangsgröße wird initialisiert
    setSceneRect(-_defaultSceneSize, -_defaultSceneSize, _defaultSceneSize, _defaultSceneSize);
}

void NetworkGraphics::mouseReleaseInterpretation(QPointF position)
{
    _mousIsPressed = false;
    _componentIsGrabbed = false;
    switch (_mouseMode)
    {
        case MouseMode::ResistorMode:
        {
            pointToGrid(&position);
            if(isThereAComponent(position))
            {
                return;
            }

            QString name = "R" + QString::number(Resistor::getCount() + 1);
            Component* resistor = new Resistor(name, 756, position.x(), position.y(), true);
            addObject(resistor);
        }
            break;
        case MouseMode::PowerSupplyMode:
        {
            pointToGrid(&position);
            if(isThereAComponent(position))
            {
                return;
            }

            if(PowerSupply::getCount() < 1)
            {
                QString name = "Q" + QString::number(PowerSupply::getCount() + 1);
                Component* powerSupply = new PowerSupply(name, position.x(), position.y(), false, 100);
                addObject(powerSupply);
            }
            else
            {
                QMessageBox::about(nullptr, "Fehleingabe", "Nur eine Spannungsquelle erlaubt");
            }
        }
            break;
        case MouseMode::ConnectionMode:
        {
            bool connectionStarted = (nullptr != _connectionStartComponentPort);
            if(connectionStarted)
            {
                ComponentPort* connectionComponentPortEnd = getComponentPortAtPosition(position);
                if(connectionComponentPortEnd != nullptr)
                {
                    Component* connectionComponentEnd = connectionComponentPortEnd->getComponent();
                    Component::Port connectionComponentEndPort = connectionComponentEnd->getPort(position);
                    ComponentPort connectionEndComponentPort = ComponentPort(connectionComponentEnd, connectionComponentEndPort);
                    addConnection(*_connectionStartComponentPort, connectionEndComponentPort);
                }
            }
        }
            break;
        case MouseMode::SelectionMode:
        {
            QApplication::setOverrideCursor(Qt::OpenHandCursor);
            pointToGrid(&position);
            _selectedItem = getComponentAtPosition(position);
            bool hasSelectedComponentToMove = (_selectedComponentToMove != nullptr);
            if(hasSelectedComponentToMove)
            {
                _selectedComponentToMove->setPosition(position);
                update();
            }
        }
            break;
        default:
            break;
    }
}

void NetworkGraphics::mousePressInterpretation(QPointF position)
{
    _mousIsPressed = true;
    QPointF memory = position;
    pointToGrid(&memory);
    _componentIsGrabbed = isThereAComponent(memory);
    if(_mouseMode != ConnectionMode)
    {
        //Keine Verbindung begonnen: _connectionStartComponentPort muss auf Nullptr zeigen
        _connectionStartComponentPort = nullptr;
    }

    switch (_mouseMode)
    {
        case ConnectionMode:
        {
            ComponentPort* foundComponentPort = getComponentPortAtPosition(position);
            if(foundComponentPort != nullptr)
            {
                _connectionStartComponentPort = foundComponentPort;
            }
        }
            break;
        case SelectionMode:
        {
            QApplication::setOverrideCursor(Qt::ClosedHandCursor);
            pointToGrid(&position);
            _selectedComponentToMove = getComponentAtPosition(position);
        }
            break;
    }
}

void NetworkGraphics::mouseDoublePressInterpretation(QPointF position)
{
    if(_mouseMode == SelectionMode)
    {
        pointToGrid(&position);
        Component* foundComponent = getComponentAtPosition(position);
        if(foundComponent != nullptr)
        {
            EditView* editingView = new EditView(foundComponent);
            editingView->show();
        }
    }
}

void NetworkGraphics::mouseMoveInterpretation(QPointF position)
{
    QColor highlightColor = QColor(136, 136, 136, 55);  //3 mal 136 ist grau und 55 ist die Transparenz

    if(nullptr != _previousRect)
    {
        removeItem(_previousRect);
        delete _previousRect;
        _previousRect = nullptr;

        update();
    }

    switch (_mouseMode)
    {
        case (ResistorMode):
        {
            highlightRect(&position, &highlightColor);
        }
        break;
        case (PowerSupplyMode):
        {
           highlightRect(&position, &highlightColor);
        }
            break;
        case ConnectionMode:
        {
            QPointF gridPosition = position;
            pointToGrid(&gridPosition);
            if(isThereAComponent(gridPosition))
            {
                ComponentPort* foundComponentPort = getComponentPortAtPosition(position);
                if(foundComponentPort != nullptr)
                {
                    Component* foundComponent = foundComponentPort->getComponent();
                    Component::Port port = foundComponentPort->getPort();

                    //TODO: Zoomfaktor einfügen
                    int hitBoxHighlight = Component::_hitBoxSize / 1.5;
                    int positionX = foundComponent->getPortPosition(port).x() - hitBoxHighlight;
                    int positionY = foundComponent->getPortPosition(port).y() - hitBoxHighlight;

                    QGraphicsItem* highlightedRect = addRect(positionX, positionY, 2 * hitBoxHighlight, 2 * hitBoxHighlight, Qt::NoPen,
                                                             highlightColor);

                    _previousRect = highlightedRect;
                    update();
                }
            }
        }
        break;
        case (SelectionMode):
        {
            if (_mousIsPressed && _componentIsGrabbed)
            {
                highlightRect(&position, &highlightColor);
            }
        }
            break;
    }
}

void NetworkGraphics::deleteItem()
{
    for(Component* component : _componentList)
    {
        if(_selectedItem == component)
        {
            _componentList.removeOne(component);
            delete component;
        }
    }
}

void NetworkGraphics::addConnection(ComponentPort componentPortA, ComponentPort componentPortB)
{
    Connection* connection = new Connection(componentPortA, componentPortB);
    _connectionList.append(connection);
    addItem(connection);
    update();
}


void NetworkGraphics::addObject(Component* component)
{
    _componentList.append(component);
    addItem(component);
    update();
}

Component* NetworkGraphics::getComponentAtPosition(QPointF gridPosition)
{
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

bool NetworkGraphics::isThereAComponent(QPointF position)
{
    return getComponentAtPosition(position) != nullptr;
}

void NetworkGraphics::pointToGrid(QPointF* position)
{
    position->setX(position->toPoint().x() / 100 * 100 - 50);
    position->setY(position->toPoint().y() / 100 * 100 - 50);
}

void NetworkGraphics::highlightRect(QPointF* position, QColor* highlightColor)
{
    pointToGrid(position);
    //TODO: Zoomfaktor einfügen
    int positionX = position->toPoint().x();
    int positionY = position->toPoint().y();
    QGraphicsItem* highlightedRect = addRect(positionX - 50, positionY - 50, 100, 100, Qt::NoPen,
                                              *highlightColor);


    _previousRect = highlightedRect;
    update();
}

ComponentPort* NetworkGraphics::getComponentPortAtPosition(QPointF position)
{
    for (Component* component : _componentList)
    {
        bool hasFoundPort = component->hasPortAtPosition(position);
        if(hasFoundPort)
        {
            Component::Port port = component->getPort(position);
            ComponentPort* cp = new ComponentPort(component, port);
            return cp;
        }
    }
    return nullptr;
}

void NetworkGraphics::calculate(void)
{
    Calculator* calculator = new Calculator(_connectionList, _componentList);
    calculator->calculate();
    QMessageBox::about(nullptr, "Berechnung", "Der Gesamtwiderstand des Netzwerkes beträgt : " +
                       QString::number(calculator->getResistanceValue()) + "Ω");
}

void NetworkGraphics::save(void)
{
    FileManager* manager = new FileManager(_componentList, _connectionList);
    manager->saving();
}

void NetworkGraphics::load(void)
{
    //Moritz hier kannst du deine FileManager-Funktion aufrufen. :)
}
