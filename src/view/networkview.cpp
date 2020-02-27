#include "networkview.h"

NetworkView::NetworkView(QWidget* parent) :
    QGraphicsView(parent)
{
    setMouseTracking(true);
}

void NetworkView::setModel(NetworkGraphics *model)
{
    _model = model;
    connect(_model, SIGNAL(newNetworkIsLoad()), this, SLOT(focus()));
}

void NetworkView::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
    QPointF scenePosition = mapToScene(mouseEvent->pos());
    QPointF gridPosition = scenePositionToGrid(scenePosition);

    //Merken des zuletzt geklickten Bereichs, wird beim Paste benötigt
    _lastClickedPositionGrid = gridPosition;

    _mouseIsPressed = false;
    _componentOrDescriptionIsGrabbed = false;

    Component::ComponentType componentType = Component::ComponentType::PowerSupply;

    switch (_mouseMode)
    {
    case MouseMode::ResistorMode:
    {
        componentType = Component::ComponentType::Resistor;
        //Durchfallen zum nächsten Case, da in beiden Fällen das Gleiche passiert
    }
    case MouseMode::PowerSupplyMode:
    {
        if (Qt::LeftButton == mouseEvent->button())
        {
            Component* createdComponent = _model->createNewComponent(gridPosition, componentType,
                                                                     _isVerticalComponentDefault);
            if (createdComponent != nullptr)
            {
                EditView* editView = new EditView(createdComponent, _model, true, this);
                editView->show();
            }
        }
        else if (Qt::RightButton == mouseEvent->button())
        {
            rotateComponent(gridPosition, scenePosition);
        }
    }
        break;
    case MouseMode::ConnectionMode:
    {
        bool connectionStarted = (nullptr != _connectionStartComponentPort);
        if (connectionStarted)
        {
            ComponentPort* connectionComponentPortEnd = nullptr;
            for (Component* component : _tempComponentListForConnections)
            {
                bool equalX = (component->getXPosition() == gridPosition.toPoint().x());
                bool equalY = (component->getYPosition() == gridPosition.toPoint().y());
                if (equalX && equalY)
                {
                    connectionComponentPortEnd = _model->getComponentPortAtPosition(scenePosition);
                }
            }
            if (connectionComponentPortEnd != nullptr)
            {
                _model->addConnection(*_connectionStartComponentPort, *connectionComponentPortEnd);
                _connectionStartComponentPort = nullptr;
            }
        }
        while (!_tempComponentListForConnections.isEmpty())
        {
          _tempComponentListForConnections.removeLast();
        }
    }
        break;
    case MouseMode::SelectionMode:
    {
        removeHighlightSelectedRect();

        _model->moveComponent(_selectedComponentToMove, _selectedDescriptionToMove, gridPosition);

        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        _selectedComponentToMove = nullptr;
        _selectedDescriptionToMove = nullptr;

        Component* foundComponent = _model->getComponentAtPosition(gridPosition);
        DescriptionField* foundDescription = _model->getDescriptionAtPosition(gridPosition);
        Connection* foundConnection = _model->getConnectionAtPosition(scenePosition);

        bool hasFoundComponent = nullptr != foundComponent;
        bool hasFoundDescription = nullptr != foundDescription;
        bool hasFoundConnection = nullptr != foundConnection;

        if (hasFoundComponent)
        {
            foundComponent->set_isSelected(true);
        }
        else if(hasFoundDescription)
        {
            foundDescription->set_isSelected(true);
        }
        else if (hasFoundConnection)
        {
            foundConnection->set_isSelected(true);
        }
    }
        break;
    case MouseMode::DescriptionMode:
    {
        if(mouseEvent->button() == Qt::LeftButton)
        {
            bool ok;
            DescriptionField * createdDescription = _model->createDescriptionField(gridPosition, false);
            if(createdDescription != nullptr)
            {
                QString text = QInputDialog::getText(this, "Textfeld bearbeiten", "Text eingeben:",QLineEdit::EchoMode::Normal, "", &ok, Qt::WindowCloseButtonHint);

                if(ok && !text.isEmpty())
                {
                    createdDescription->setText(text);
                }
            }
        }
    }
        break;
    default:
        break;
    }
}

void NetworkView::mousePressEvent(QMouseEvent* event)
{
    QPointF scenePosition = mapToScene(event->pos());

    _mouseIsPressed = true;
    QPointF gridPosition = scenePositionToGrid(scenePosition);
    _componentOrDescriptionIsGrabbed = _model->isThereAComponentOrADescription(gridPosition);

    //Keine Verbindung begonnen: _connectionStartComponentPort muss auf Nullptr zeigen
    _connectionStartComponentPort = nullptr;
    _selectedComponentToMove = nullptr;

    switch (_mouseMode)
    {
    case ConnectionMode:
    {
        _connectionStartComponentPort = _model->getComponentPortAtPosition(scenePosition);
        Component* tempComponentForConnection = _model->getComponentAtPosition(gridPosition);
        _tempComponentListForConnections = _model->getComponents();
        //qDebug() << _tempComponentListForConnections;
        _tempComponentListForConnections.removeOne(tempComponentForConnection);
        //qDebug() << _tempComponentListForConnections;


    }
        break;
    case SelectionMode:
    {
        bool isComponentAtPosition = _model->isThereAComponentOrADescription(gridPosition);
        if (isComponentAtPosition)
        {
            QApplication::setOverrideCursor(Qt::ClosedHandCursor);
            _selectedComponentToMove = _model->getComponentAtPosition(gridPosition);
            _selectedDescriptionToMove = _model->getDescriptionAtPosition(gridPosition);

            if(_selectedComponentToMove != nullptr)
            {
                _selectedDescriptionToMove = nullptr;
            }
            else
            {
                _selectedComponentToMove = nullptr;
            }
        }
        else
        {
            _selectedComponentToMove = nullptr;
            _selectedDescriptionToMove = nullptr;
        }
    }
        break;
    case ResistorMode:
    {
    }
        break;
    case PowerSupplyMode:
    {
    }
        break;
    case DescriptionMode:
    {
    }
        break;
    }
}

void NetworkView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF scenePosition = mapToScene(event->pos());
    QPointF gridPosition = scenePositionToGrid(scenePosition);

    //Notwendig für den Rechtsklick per Shortcut
    _actualMoveScenePosition = scenePosition;
    //MemoryLeak vermeiden
    gridDisappears();

    switch (_mouseMode)
    {
    case (ResistorMode):
    {
        if(!_model->isThereAComponentOrADescription(gridPosition))
        {
            Component* sampleResistor = new Resistor(QString("R"), 0, gridPosition.toPoint().x(),
                                                     gridPosition.toPoint().y(), _isVerticalComponentDefault, 0);
            _sampleComponentOnMoveEvent = sampleResistor;
            _model->addItem(_sampleComponentOnMoveEvent);
            highlightRect(scenePosition, _highlightColor);
        }
    }
        break;
    case (PowerSupplyMode):
    {
        if(!_model->isThereAComponentOrADescription(gridPosition))
        {
            Component* sampleResistor = new PowerSupply(QString("Q"), gridPosition.toPoint().x(),
                                                        gridPosition.toPoint().y(), _isVerticalComponentDefault, 0);
            _sampleComponentOnMoveEvent = sampleResistor;
            _model->addItem(_sampleComponentOnMoveEvent);
            highlightRect(scenePosition, _highlightColor);
        }
    }
        break;
    case ConnectionMode:
    {
        //TODO: nicht auf GridPosition beschränken, sondern Model fragen, ob an scenePosition ein Port in der Nähe ist
        if (_model->isThereAComponentOrADescription(gridPosition))
        {
            ComponentPort* foundComponentPort = _model->getComponentPortAtPosition(scenePosition);
            if (foundComponentPort != nullptr)
            {
                Component* foundComponent = foundComponentPort->getComponent();
                Component::Port port = foundComponentPort->getPort();

                int hitBoxHighlight = Component::_hitBoxSize / 1.5;
                int positionX = foundComponent->getPortPosition(port).x() - hitBoxHighlight;
                int positionY = foundComponent->getPortPosition(port).y() - hitBoxHighlight;

                QGraphicsItem* highlightedRect = _model->addRect(positionX, positionY, 2 * hitBoxHighlight,
                                                                 2 * hitBoxHighlight, Qt::NoPen,
                                                                 _highlightColor);

                _previousHighlightedRect = highlightedRect;
                _model->update();
            }
        }
    }
        break;
    case SelectionMode:
    {
        if (_mouseIsPressed && _componentOrDescriptionIsGrabbed)
        {
            highlightRect(scenePosition, _highlightColor);
        }
    }
        break;
    case DescriptionMode:
    {
        if(!_model->isThereAComponentOrADescription(gridPosition))
        {
            _sampleDescriptionOnMoveEvent = new DescriptionField(gridPosition.x(), gridPosition.y(),0);
            _model->addItem(_sampleDescriptionOnMoveEvent);
            highlightRect(scenePosition, _highlightColor);
        }
    }
        break;
    }
}

void NetworkView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPointF scenePosition = mapToScene(event->pos());

    if (_mouseMode == SelectionMode)
    {
        QPointF gridPosition = scenePositionToGrid(scenePosition);

        for(Component* component : _model->getComponents())
        {
            if(component->isSelected())
            {
                _model->getComponentAtPosition(gridPosition);
            }
        }

        for(DescriptionField* description : _model->getDescriptions())
        {
            if(description->isSelected())
            {
                _model->getDescriptionAtPosition(gridPosition);
            }
        }
            //TODO: emit foundComponent
        editNetworkOrDescription();
    }
}

QPointF NetworkView::scenePositionToGrid(QPointF scenePosition)
{
    qreal xPos = scenePosition.toPoint().x() / 100 * 100 - 50;
    qreal yPos = scenePosition.toPoint().y() / 100 * 100 - 50;
    return QPointF(xPos, yPos);
}

void NetworkView::gridDisappears(void)
{
    //Grid verschwindet
    if (nullptr != _previousHighlightedRect)
    {
        _model->removeItem(_previousHighlightedRect);
        delete _previousHighlightedRect;
        _previousHighlightedRect = nullptr;

        _model->update();
    }

    if (nullptr != _sampleComponentOnMoveEvent)
    {
        _model->removeItem(_sampleComponentOnMoveEvent);
        delete _sampleComponentOnMoveEvent;
        _sampleComponentOnMoveEvent = nullptr;

        _model->update();
    }

    if(nullptr != _sampleDescriptionOnMoveEvent)
    {
        _model->removeItem(_sampleDescriptionOnMoveEvent);
        delete _sampleDescriptionOnMoveEvent;
        _sampleDescriptionOnMoveEvent = nullptr;

        _model->update();
    }
}

void NetworkView::highlightRect(QPointF scenePosition, QColor highlightColor)
{
    QPointF gridPosition = scenePositionToGrid(scenePosition);
    int positionX = gridPosition.toPoint().x();
    int positionY = gridPosition.toPoint().y();
    if (!_model->isThereAComponentOrADescription(gridPosition))
    {
        QGraphicsItem* highlightedRect = _model->addRect(positionX - 50, positionY - 50, 100, 100, Qt::NoPen,
                                                         highlightColor);
        _previousHighlightedRect = highlightedRect;

        _model->update();
    }
}

/*!
 * \brief Entfernt ein ausgewähltes Element aus der Zeichenebene.
 *
 * Anhand des Types des Elements, werden diese entsprechend enfernt.
 */
void NetworkView::deleteSelectedItem(void)
{
    for(Component* component : _model->getComponents())
    {
        if(component->isSelected())
        {

            _model->deleteComponentWithoutUndo(component);
            _copiedComponent = nullptr;
        }
    }

    for(DescriptionField* description : _model->getDescriptions())
    {
        if(description->isSelected())
        {
            _model->deleteDescription(description);
            _copiedDescription = nullptr;
        }
    }

    for(Connection* connection : _model->getConnections())
    {
        if(connection->isSelected())
        {
            _model->deleteConnectionWithoutUndo(connection);
        }
    }
    removeHighlightSelectedRect();
}

/*!
 * \brief Bearbeiten des Netzwerkes oder eines Textfeldes.
 *
 *
 */
void NetworkView::editNetworkOrDescription()
{
    for(Component* component : _model->getComponents())
    {
        if(component->isSelected())
        {
            EditView* editView = new EditView(component, _model, false, this);
            editView->show();
        }
    }

    for(DescriptionField* description : _model->getDescriptions())
    {
        if(description->isSelected())
        {
            bool ok = false;
            QString text = QInputDialog::getText(this, "Textfeld bearbeiten", "Text eingeben:",QLineEdit::EchoMode::Normal, description->getText(), &ok, Qt::WindowCloseButtonHint);
            if(ok)
            {
                description->setText(text);
            }
        }
    }
}

//Fokussiert auf das geladene Netzwerk
void NetworkView::focus()
{
    centerOn(findScrollPosition());
}

void NetworkView::removeHighlightSelectedRect(void)
{
    for(Component* component : _model->getComponents())
    {
        component->set_isSelected(false);
    }

    for(DescriptionField* descriptionfield : _model->getDescriptions())
    {
        descriptionfield->set_isSelected(false);
    }

    for(Connection* connection : _model->getConnections())
    {
            connection->set_isSelected(false);
    }

    _model->update();
}

void NetworkView::rotateComponent(QPointF gridPosition, QPointF scenePosition)
{
    _isVerticalComponentDefault = !_isVerticalComponentDefault;
    gridDisappears();
    if(!_model->isThereAComponentOrADescription(gridPosition))
    {
        if (MouseMode::PowerSupplyMode == _mouseMode)
        {
            Component* sampleResistor = new PowerSupply(QString("Q"), gridPosition.toPoint().x(),
                                                        gridPosition.toPoint().y(), _isVerticalComponentDefault,
                                                        0);
            _sampleComponentOnMoveEvent = sampleResistor;
        }
        else if (MouseMode::ResistorMode == _mouseMode)
        {
            Component* sampleResistor = new Resistor(QString("R"), 0, gridPosition.toPoint().x(),
                                                     gridPosition.toPoint().y(), _isVerticalComponentDefault,
                                                     0);
            _sampleComponentOnMoveEvent = sampleResistor;
        }
    }
    _model->addItem(_sampleComponentOnMoveEvent);
    highlightRect(scenePosition, _highlightColor);
}

bool NetworkView::lookingForFreeSpaceToDuplicate(int xPos, int yPos, int &xWaytoTheRight)
{
    bool created = false;
    while (!created)
    {
        if (!_model->isThereAComponentOrADescription(
                    QPointF(xPos + xWaytoTheRight, yPos)))
        {
            created = true;
            return created;
        }

        xWaytoTheRight += 100;
    }
    return created;
}

//Sucht nach der DurchschnittsXPosition und der DurchschnittsYPosition
QPointF NetworkView::findScrollPosition()
{
    int averageX = 0;
    int averageY = 0;
    int anzahlResistorAndDescriptions = 0;
    if(_model->getComponents().count() != 0)
    {
        for(Component* c : _model->getComponents())
        {
            if(c->getComponentType() == Component::Resistor)
            {
                //qDebug() << c->getName();
                averageX += c->getXPosition();
                averageY += c->getYPosition();

                anzahlResistorAndDescriptions++;
            }
        }
    }
    if(_model->getDescriptions().count() != 0)
    {
        for(DescriptionField* c : _model->getDescriptions())
        {
            averageX += c->getXPos();
            averageY += c->getYPos();
            anzahlResistorAndDescriptions++;
        }
    }
    qDebug() << anzahlResistorAndDescriptions;
    averageY /= anzahlResistorAndDescriptions;
    averageX /= anzahlResistorAndDescriptions;
    return QPointF(averageX, averageY);
}

//Bereitet die Scene und GridPosition für das Rotieren per Shortcut auf
void NetworkView::rotateComponentByShortcut()
{
    QPointF gridPosition = scenePositionToGrid(_actualMoveScenePosition);
    rotateComponent(gridPosition, _actualMoveScenePosition);
}

/*!
 * \brief Dupliziert eine Komponente.
 *
 * Die ausgewählte Komponen wird in ein freies Feld nach rechts dupliziert.
 */
void NetworkView::duplicate(void)
{
    for(Component* component : _model->getComponents())
    {
        if(component->isSelected())
        {
            int xWayToTheRight = 100;
            if(lookingForFreeSpaceToDuplicate(component->getXPosition(), component->getYPosition(), xWayToTheRight))
                _model->duplicateComponent(component, component->getXPosition() + xWayToTheRight,
                                           component->getYPosition());
        }
    }

    for(DescriptionField* description : _model->getDescriptions())
    {
        if(description->isSelected())
        {
            int xWayToTheRight = 100;
            if(lookingForFreeSpaceToDuplicate(description->getXPos(), description->getYPos(), xWayToTheRight))
            {
                _model->duplicateDescription(description, description->getXPos() + xWayToTheRight,
                                             description->getYPos());
            }
        }
    }
}

/*!
 * \brief Kopiert die ausgewählte Komponente oder Textfeld.
 *
 * Die ausgewählte Komponente oder Textfeld wird kopiert.
 */
void NetworkView::copy(void)
{
    for(Component* component : _model->getComponents())
    {
        if(component->isSelected())
        {
            _copiedComponent = component;
            _copiedDescription = nullptr;
        }
    }

    for(DescriptionField* description : _model->getDescriptions())
    {
        if(description->isSelected())
        {
            _copiedDescription = description;
            _copiedComponent = nullptr;
        }
    }
}

/*!
 * \brief Fügt eine Komponente oder ein Textfeld ein.
 *
 * Wenn sich an der ausgewählten neuen Position keine Komponente oder Textfeld befindet, wird dieses dort eingefügt.
 */
void NetworkView::paste(void)
{
    if (!_model->isThereAComponentOrADescription(_lastClickedPositionGrid))
    {
        if(_copiedComponent != nullptr)
        {
            _model->duplicateComponent(_copiedComponent, _lastClickedPositionGrid.x(), _lastClickedPositionGrid.y());
        }
        else if(_copiedDescription != nullptr)
        {
            _model->duplicateDescription(_copiedDescription,_lastClickedPositionGrid.x(), _lastClickedPositionGrid.y());
        }
    }
}

void NetworkView::enterEvent(QEvent* event)
{
    switch (_mouseMode)
    {
    case ConnectionMode:
    {
        QApplication::setOverrideCursor(Qt::CrossCursor);
    }
        break;
    case SelectionMode:
    {
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
    }
        break;
    case PowerSupplyMode:
    {
    }
        break;
    case ResistorMode:
    {
    }
        break;
    case DescriptionMode:
    {
    }
        break;
    }
    QWidget::enterEvent(event);
}

void NetworkView::leaveEvent(QEvent* event)
{
    QApplication::setOverrideCursor(Qt::CustomCursor);
    QWidget::leaveEvent(event);

    gridDisappears();
}

void NetworkView::keyPressEvent(QKeyEvent* event)
{
    //TODO: Dies muss ins Mainwindow
    if (event->key() == Qt::Key_Escape)
    {
        setMouseMode(NetworkView::MouseMode::SelectionMode);
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        removeHighlightSelectedRect();
        gridDisappears();

        //Alle selected Objekte auf nullptr setzten, sonst ungewolltes Löschen
        for(Component* component : _model->getComponents())
        {
            if(component->isSelected())
            {
                component->set_isSelected(false);
            }
        }

        for(DescriptionField* description : _model->getDescriptions())
        {
            if(description->isSelected())
            {
                description->set_isSelected(false);
            }
        }

        for(Connection* connection : _model->getConnections())
        {
            if(connection->isSelected())
            {
                connection->set_isSelected(false);
            }
        }
    }
    QGraphicsView::keyPressEvent(event);
}

