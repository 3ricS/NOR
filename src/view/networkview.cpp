#include "networkview.h"

NetworkView::NetworkView(QWidget* parent) :
        QGraphicsView(parent)
{
}

void NetworkView::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
    QPointF scenePosition = mapToScene(mouseEvent->pos());
    QPointF gridPosition = scenePositionToGrid(scenePosition);

    _mouseIsPressed = false;
    _componentIsGrabbed = false;

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
                Component* createdComponent = _model->createNewComponent(mouseEvent, gridPosition, componentType,
                                                                         _isVerticalComponentDefault);
                if (createdComponent != nullptr)
                {
                    EditView* editView = new EditView(createdComponent, _model, true);
                    editView->show();
                }
            }
            else if (Qt::RightButton == mouseEvent->button())
            {
                _isVerticalComponentDefault = !_isVerticalComponentDefault;
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
            bool isComponentAtPosition = _model->isThereAComponent(scenePosition);
            bool hasSelectedComponentToMove = (_selectedComponentToMove != nullptr);

            if (isComponentAtPosition)
            {
                highlightSelectedRect(gridPosition);
            }
            else if (hasSelectedComponentToMove)
            {
                //TODO: alten Component entfernen
                _selectedComponentToMove->setPosition(gridPosition);
                _isDragged = true;
                highlightSelectedRect(gridPosition);
                _model->update();
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
    _componentIsGrabbed = _model->isThereAComponent(gridPosition);

    if (_mouseMode != ConnectionMode)
    {
        //Keine Verbindung begonnen: _connectionStartComponentPort muss auf Nullptr zeigen
        _connectionStartComponentPort = nullptr;
    }

    //MemoryLeak vermeiden
    if (!_model->isThereAComponent(scenePosition))
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
            QApplication::setOverrideCursor(Qt::ClosedHandCursor);
            _selectedComponentToMove = _model->getComponentAtPosition(gridPosition);
        }
            break;
    }
}

void NetworkView::mouseMoveEvent(QMouseEvent* event)
{
    setMouseTracking(true);
    QPointF scenePosition = mapToScene(event->pos());

    QColor highlightColor = QColor(136, 136, 136, 55);  //3 mal 136 ist grau und 55 ist die Transparenz

    //MemoryLeak vermeiden
    if (nullptr != _previousRect)
    {
        _model->removeItem(_previousRect);
        delete _previousRect;
        _previousRect = nullptr;

        _model->update();
    }
    if (nullptr != _sampleComponentOnMoveEvent)
    {
        _model->removeItem(_sampleComponentOnMoveEvent);
        delete _sampleComponentOnMoveEvent;
        _sampleComponentOnMoveEvent = nullptr;

        _model->update();
    }

    switch (_mouseMode)
    {
        case (ResistorMode):
        {
            QPointF gridPosition = scenePositionToGrid(scenePosition);
            Component* sampleResistor = new Resistor(QString("R"), 0, gridPosition.toPoint().x(),
                                                     gridPosition.toPoint().y(), _isVerticalComponentDefault);
            _sampleComponentOnMoveEvent = sampleResistor;
            _model->addItem(_sampleComponentOnMoveEvent);
            highlightRect(scenePosition, highlightColor);
        }
            break;
        case (PowerSupplyMode):
        {
            QPointF gridPosition = scenePositionToGrid(scenePosition);
            Component* sampleResistor = new PowerSupply(QString("Q"), gridPosition.toPoint().x(),
                                                        gridPosition.toPoint().y(), _isVerticalComponentDefault);
            _sampleComponentOnMoveEvent = sampleResistor;
            _model->addItem(_sampleComponentOnMoveEvent);
            highlightRect(scenePosition, highlightColor);
        }
            break;
        case ConnectionMode:
        {
            //TODO: nicht auf GridPosition beschränken, sondern Model fragen, ob an scenePosition ein Port in der Nähe ist
            QPointF gridPosition = scenePositionToGrid(scenePosition);
            if (_model->isThereAComponent(gridPosition))
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
                                                                     highlightColor);

                    _previousRect = highlightedRect;
                    _model->update();
                }
            }
        }
            break;
        case (SelectionMode):
        {
            if (_mouseIsPressed && _componentIsGrabbed)
            {
                highlightRect(scenePosition, highlightColor);
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
        Component* foundComponent = _model->getComponentAtPosition(gridPosition);
        if (foundComponent != nullptr)
        {
            _selectedComponent = foundComponent;
            //TODO: emit foundComponent
            EditView* editView = new EditView(foundComponent, _model);
            editView->show();
        }
    }
}

QPointF NetworkView::scenePositionToGrid(QPointF scenePosition)
{
    qreal xPos = scenePosition.toPoint().x() / 100 * 100 - 50;
    qreal yPos = scenePosition.toPoint().y() / 100 * 100 - 50;
    return QPointF(xPos, yPos);
}

void NetworkView::highlightSelectedRect(QPointF gridPosition)
{
    int positionX;
    int positionY;
    if (_isDragged)
    {
        //_selectedComponentToMove darf nicht auf einen anderen Widerstand bewegt werden
        positionX = _selectedComponentToMove->getXPosition();
        positionY = _selectedComponentToMove->getYPosition();
        _isDragged = 0;
    }
    _selectedComponent = _model->getComponentAtPosition(gridPosition);
    if (nullptr != _selectedRect)
    {
        _model->removeItem(_selectedRect);
        delete _selectedRect;
        _selectedRect = nullptr;
    }
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
    if (!_model->isThereAComponent(gridPosition))
    {
        QGraphicsItem* highlightedRect = _model->addRect(positionX - 50, positionY - 50, 100, 100, Qt::NoPen,
                                                         highlightColor);
        _previousRect = highlightedRect;

        _model->update();
    }
}

