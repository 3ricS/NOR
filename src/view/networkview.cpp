#include "networkview.h"

NetworkView::NetworkView(QWidget* parent) :
    QGraphicsView(parent)
{
    setMouseTracking(true);
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
            ComponentPort* connectionComponentPortEnd = _model->getComponentPortAtPosition(scenePosition);
            if (connectionComponentPortEnd != nullptr)
            {
                _model->addConnection(*_connectionStartComponentPort, *connectionComponentPortEnd);
                _connectionStartComponentPort = nullptr;
            }
        }
    }
        break;
    case MouseMode::SelectionMode:
    {
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        _selectedComponentToMove = nullptr;
        _selectedDescriptionToMove = nullptr;

        Component* foundComponent = _model->getComponentAtPosition(gridPosition);
        DescriptionField* foundDescription = _model->getDescriptionAtPosition(gridPosition);

        bool hasFoundComponent = nullptr != foundComponent;
        bool hasFoundDescription = nullptr != foundDescription;

        if (hasFoundComponent)
        {
            _selectedComponent = foundComponent;
            highlightSelectedRect(gridPosition);
            _selectedDescription = nullptr;
        }
        else if(hasFoundDescription)
        {
            _selectedDescription = foundDescription;
            highlightSelectedRect(gridPosition);
            _selectedComponent = nullptr;
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
                QString text = QInputDialog::getText(this, "text", "Text",QLineEdit::EchoMode::Normal, "Beschreibung", &ok);

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

    //MemoryLeak vermeiden
    if (!_model->isThereAComponentOrADescription(scenePosition))
    {
        if (nullptr != _selectedRect)
        {
            _model->removeItem(_selectedRect);
            delete _selectedRect;
            _selectedRect = nullptr;
        }
    }

    switch (_mouseMode)
    {
    case ConnectionMode:
    {
        _connectionStartComponentPort = _model->getComponentPortAtPosition(scenePosition);
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
            qDebug() << "hallo";
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
    case (SelectionMode):
    {
        if (_mouseIsPressed && _componentOrDescriptionIsGrabbed)
        {
            highlightRect(scenePosition, _highlightColor);
        }

        _model->moveComponent(_selectedComponentToMove, _selectedDescriptionToMove ,gridPosition);
    }
        break;
    case DescriptionMode:
    {
        _sampleDescriptionOnMoveEvent = new DescriptionField(gridPosition.x(), gridPosition.y(),0);
        _model->addItem(_sampleDescriptionOnMoveEvent);
        highlightRect(scenePosition, _highlightColor);
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
        Component* foundComponent = _model->getComponentAtPosition(gridPosition);
        if (foundComponent != nullptr)
        {
            _selectedComponent = foundComponent;
            //TODO: emit foundComponent
            EditView* editView = new EditView(foundComponent, _model, false, this);
            editView->show();
        }

        DescriptionField* description = _model->getDescriptionAtPosition(gridPosition);
        if(description != nullptr)
        {
            bool ok = false;
            QString text = QInputDialog::getText(this, "text", "Text",QLineEdit::EchoMode::Normal, description->getText(), &ok);
            if(ok)
            {
                description->setText(text);
            }
        }
    }
}

QPointF NetworkView::scenePositionToGrid(QPointF scenePosition)
{
    qreal xPos = scenePosition.toPoint().x() / 100 * 100 - 50;
    qreal yPos = scenePosition.toPoint().y() / 100 * 100 - 50;
    return QPointF(xPos, yPos);
}

void NetworkView::gridDisappears()
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
        delete _sampleComponentOnMoveEvent;
        _sampleComponentOnMoveEvent = nullptr;

        _model->update();
    }
}

void NetworkView::highlightSelectedRect(QPointF gridPosition)
{
    int positionX;
    int positionY;

    //TODO: das Markieren des selectedComponent gehört nicht in die Methode für das Highlighting des Bereichs
    _selectedComponent = _model->getComponentAtPosition(gridPosition);

    removeHighlightSelectedRect();

    QColor highlightColor = QColor(255, 0, 0, 55);
    positionX = gridPosition.toPoint().x();
    positionY = gridPosition.toPoint().y();
    QGraphicsItem* highlightSelectedRect = _model->addRect(positionX - 50, positionY - 50, 100, 100, Qt::NoPen,
                                                           highlightColor);

    _selectedRect = highlightSelectedRect;
    _model->update();
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

void NetworkView::deleteSelectedItem()
{
    removeHighlightSelectedRect();

    if(_selectedComponent != nullptr)
    {
        _model->deleteComponent(_selectedComponent);

        if (_copiedComponent == _selectedComponent)
        {
            _copiedComponent = nullptr;
        }

        _selectedComponent = nullptr;
    }
    else if(_selectedDescription != nullptr)
    {
        _model->deleteDescription(_selectedDescription);

        if(_copiedDescription == _selectedDescription)
        {
            _copiedDescription = nullptr;
        }
        _selectedDescription = nullptr;
    }
}

//Wenn ESC gedrückt wird, wird der Mouse Modus aud Selection geändert
void NetworkView::EscapeKeyPressed(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        setMouseMode(NetworkView::MouseMode::SelectionMode);
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        removeHighlightSelectedRect();
        gridDisappears();
    }
    if (event->key() == Qt::Key_Delete)
    {
        deleteSelectedItem();
    }
}

void NetworkView::removeHighlightSelectedRect()
{
    if (nullptr != _selectedRect)
    {
        _model->removeItem(_selectedRect);
        delete _selectedRect;
        _selectedRect = nullptr;
    }
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

//Bereitet die Scene und GridPosition für das Rotieren per Shortcut auf
void NetworkView::rotateComponentByShortcut()
{
    QPointF gridPosition = scenePositionToGrid(_actualMoveScenePosition);
    rotateComponent(gridPosition, _actualMoveScenePosition);
}

void NetworkView::duplicate()
{
    // Verschiebt so lange nach rechts, bis er auf eine Grid-Position gestoßen ist, die unbelegt ist
    if (_selectedComponent != nullptr)
    {
        int xWayToTheRight = 100;
        if(lookingForFreeSpaceToDuplicate(_selectedComponent->getXPosition(), _selectedComponent->getYPosition(), xWayToTheRight))
        _model->duplicateComponent(_selectedComponent, _selectedComponent->getXPosition() + xWayToTheRight,
                                   _selectedComponent->getYPosition());
    }
    else if(_selectedDescription != nullptr)
    {
        int xWayToTheRight = 100;
        if(lookingForFreeSpaceToDuplicate(_selectedDescription->getXPos(), _selectedDescription->getYPos(), xWayToTheRight))
        {
            _model->duplicateDescription(_selectedDescription, _selectedDescription->getXPos() + xWayToTheRight,
                                         _selectedDescription->getYPos());
        }
    }
}

void NetworkView::copy()
{
    if(_selectedComponent != nullptr)
    {
        _copiedComponent = _selectedComponent;
        _copiedDescription = nullptr;
    }
    else if(_selectedDescription != nullptr)
    {
        _copiedDescription = _selectedDescription;
        _copiedComponent = nullptr;
    }
}

void NetworkView::paste()
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

