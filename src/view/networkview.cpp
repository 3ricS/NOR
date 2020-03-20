#include "defines.h"
#include "networkview.h"

#include <QPrintDialog>
#include <QDebug>
#include <QPrinter>

NetworkView::NetworkView(QWidget* parent) :
        QGraphicsView(parent)
{
    setMouseTracking(true);
}

/*!
 * \brief Verbindet den View mit dem Model
 *
 * \param   model ist das zu setzende Model
 *
 * Der View bekommt das Model übergeben, damit dieses bekannt ist.
 */
void NetworkView::setModel(NetworkGraphics* model)
{
    _model = model;
    connect(_model, SIGNAL(newNetworkIsLoad()), this, SLOT(focus()));
}

void NetworkView::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
    QPointF scenePosition = mapToScene(mouseEvent->pos());

    //Merken des zuletzt geklickten Bereichs, wird beim Paste benötigt
    _lastClickedPosition = scenePosition;
    _mouseIsPressed = false;
    updateOverrideCursor();

    switch (_mouseMode)
    {
        case MouseMode::ResistorMode:
        case MouseMode::PowerSupplyMode:
        {
            if (Qt::LeftButton == mouseEvent->button())
            {
                addInitialComponentWithMouseMode(scenePosition, _mouseMode);
            }
            else if (Qt::RightButton == mouseEvent->button())
            {
                rotateComponent(scenePosition);
            }
        }
            break;
        case MouseMode::ConnectionMode:
        {
            buildConnection(scenePosition);
            deleteConnectionHighlights();
        }
            break;
        case MouseMode::SelectionMode:
        {
            bool hasStartedMoving = moveIfNeeded(scenePosition);
            if (!hasStartedMoving)
            {
                multiselect(scenePosition, true);
            }
        }
            break;
        case MouseMode::DescriptionMode:
        {
            if (mouseEvent->button() == Qt::LeftButton)
            {
                addInitialDescription(scenePosition);
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

    //Keine Verbindung begonnen: _connectionStartComponentPort muss auf Nullptr zeigen
    _connectionStartComponentPort = nullptr;
    _selectedObjectToMove = nullptr;

    if (ConnectionMode == _mouseMode)
    {
        startConnection(scenePosition);
    }
    else if (SelectionMode == _mouseMode)
    {
        startSelectionOrMove(scenePosition);
    }
}

void NetworkView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF scenePosition = mapToScene(event->pos());

    //Notwendig für den Rechtsklick per Shortcut
    _actualMoveScenePosition = scenePosition;
    //MemoryLeak vermeiden
    deleteSampleObjectsAndHighlights();

    switch (_mouseMode)
    {
        case (ResistorMode):
        case (PowerSupplyMode):
        {
            showSampleComponent(scenePosition, _mouseMode);
        }
            break;
        case ConnectionMode:
        {
            connectionMoveEvent(scenePosition);
        }
            break;
        case SelectionMode:
        {
            if (_mouseIsPressed && _selectedObjectToMove == nullptr)
            {
                multiselect(scenePosition, false);
            }
            else if (_mouseIsPressed && _selectedObjectToMove != nullptr)
            {
                highlightGrid(scenePosition, Defines::highlightColor);
            }
        }
            break;
        case DescriptionMode:
        {
            showSampleDescription(scenePosition);
        }
            break;
    }
}

void NetworkView::mouseDoubleClickEvent([[maybe_unused]]QMouseEvent* event)
{
    if (_mouseMode == SelectionMode)
    {
        editGridObject();
    }
}

void NetworkView::deleteSampleObjectsAndHighlights(void)
{
    _previousHighlightedRect = deleteGraphicsItem(_previousHighlightedRect);
    _sampleComponent = dynamic_cast<Component*>(deleteGraphicsItem(_sampleComponent));
    _sampleDescription = dynamic_cast<Description*>(deleteGraphicsItem(_sampleDescription));
}

#include <QDebug>

void NetworkView::highlightGrid(QPointF scenePosition, QColor highlightColor)
{
    QPointF gridPosition = _model->mapSceneToGrid(scenePosition);
    int positionX = gridPosition.toPoint().x();
    int positionY = gridPosition.toPoint().y();

    if (!_model->hasObjectAtPosition(gridPosition))
    {
        QPointF startOfRect = QPointF(positionX - (Defines::gridLength / 2), positionY - (Defines::gridLength / 2));
        QGraphicsItem* highlightedRect = _model->addRect(startOfRect.x(), startOfRect.y(),
                                                         Defines::gridLength, Defines::gridLength, Qt::NoPen,
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
void NetworkView::deleteSelectedItems(void)
{
    _model->deleteSelectedObjects(_copiedObjects);
}

/*!
 * \brief Bearbeiten des Netzwerkes oder eines Textfeldes.
 *
 * Es werden entweder eine Komponente oder ein Textfeld in dem Netzwerk bearbeitet.
 */
void NetworkView::editGridObject(void)
{
    QList<GridObject*> selectedObjects = _model->getSelectedObjects();

    for (GridObject* gridObject : selectedObjects)
    {
        Component* component = dynamic_cast<Component*>(gridObject);
        Description* description = dynamic_cast<Description*>(gridObject);

        if (nullptr != component)
        {
            EditView* editView = new EditView(component, _model, false, this, _model->getUndoStack());
            editView->show();
        }
        else if (nullptr != description)
        {
            bool ok = false;
            QString text = QInputDialog::getMultiLineText(this, "Textfeld bearbeiten", "Text eingeben:",
                                                          description->getText(), &ok, Qt::WindowCloseButtonHint);
            if (ok)
            {
                _model->editDescription(description, text);
            }
        }
    }
}

//Fokussiert auf das geladene Netzwerk
void NetworkView::focus(void)
{
    centerOn(findScrollPosition());
}

void NetworkView::rotateComponent(QPointF scenePosition)
{
    if (MouseMode::SelectionMode != _mouseMode)
    {
        _isVerticalComponentDefault = !_isVerticalComponentDefault;
        showSampleComponent(scenePosition, _mouseMode);
    }
    else if (MouseMode::SelectionMode == _mouseMode)
    {
        _model->turnSelectedComponentsRight();
    }
}

//Sucht nach der DurchschnittsXPosition und der DurchschnittsYPosition
QPointF NetworkView::findScrollPosition(void)
{
    int averageX = 0;
    int averageY = 0;
    int anzahlResistorAndDescriptions = 0;
    if (_model->getComponents().count() != 0)
    {
        for (Component* c : _model->getComponents())
        {
            if (c->getComponentTypeInt() == Component::Resistor)
            {
                averageX += c->getPosition().x();
                averageY += c->getPosition().y();

                anzahlResistorAndDescriptions++;
            }
        }
    }
    if (_model->getDescriptions().count() != 0)
    {
        for (Description* c : _model->getDescriptions())
        {
            averageX += c->getPosition().x();
            averageY += c->getPosition().y();
            anzahlResistorAndDescriptions++;
        }
    }
    if (anzahlResistorAndDescriptions != 0)
    {
        averageY /= anzahlResistorAndDescriptions;
        averageX /= anzahlResistorAndDescriptions;
    }
    _model->update();
    return QPointF(averageX, averageY);
}

void NetworkView::focusForPrint(void)
{
    QPointF center = findScrollPosition();
    center.setY(center.y() - Defines::gridLength);
    centerOn(center);
}

/*!
 * \brief Bereitet die Scene und GridPosition für das Rotieren per Shortcut auf.
 */
void NetworkView::rotateComponentByShortcut(void)
{
    rotateComponent(_actualMoveScenePosition);
}

/*!
 * \brief Ausdrucken der Netzwerkschaltung.
 *
 * Es werden die Standardeigenschaften zum drucken gesetzt, wie die größe des Blattes und die Ausrichtung.
 * Es folgt das fokussieren des Netzwerkes.
 * Es wird die Schaltung, der Gesamtwiderstandswert der Schaltung und das Copyright gedruckt.
 */
void NetworkView::print(void)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);
    printer.setFullPage(true);

    QPrintDialog dlg(&printer);
    if (dlg.exec() == QDialog::Accepted)
    {
        //Zuerst fokussieren
        focusForPrint();

        //Alle Objekte im View werden gezeichnet
        QPainter p(&printer);
        render(&p);

        //Name der Schaltung
        QString name;
        if (_model->getFileName() == "")
        {
            name = "Widerstandsnetzwerk";
        }
        else
        {
            QStringList splittedName = _model->getFileName().split('.');

            //Nur das erste Item interessiert, da zweites die Dateiendung ist
            //Datei-Name mit Punkten sollten eh nicht gemacht werden - erzieherische Maßnahme für diejenigen die es dennoch tun
            name = splittedName.at(0);
        }

        p.drawText(QPointF(950, 50), name);

        //Gesamtwiderstand wird gedruckt
        p.drawText(QPointF(750, 3000),
                   "Gesamtwiderstand der Schaltung: " + QString::number(_model->getResistanceValue(), 'f', 2) + "Ω");

        //Copyright wird gedruckt
        QFont f;
        f.setPixelSize(18);
        p.setFont(f);
        p.drawText(QPointF(0, 3500), "© NOR-Developer-Team");

        p.end();
    }
}

/*!
 * \brief Ausschneiden einer Komponente oder eines Textfeldes.
 */
void NetworkView::cut(void)
{
    _copiedObjects = _model->getSelectedObjects();
    _model->cutObjects(_copiedObjects);
}

/*!
 * \brief Setzt den Mausmodus
 *
 * \param newMode ist der neue Mausmodus
 */
void NetworkView::setMouseMode(NetworkView::MouseMode newMode)
{
    if (isAllowedToChangeMode())
    {
        _mouseMode = newMode;
        updateOverrideCursor();
        deleteSampleObjectsAndHighlights();
    }
}

/*!
 * \brief Dupliziert eine Komponente.
 *
 * Die ausgewählte Komponente wird in ein freies Feld nach rechts dupliziert.
 */
void NetworkView::duplicate(void)
{
    _model->duplicateSelectedGridObjects();
}

/*!
 * \brief Kopiert die ausgewählte Komponente oder Textfeld.
 *
 * Die ausgewählte Komponente oder Textfeld wird kopiert.
 */
void NetworkView::copy(void)
{
    _copiedObjects.clear();
    _copiedObjects = _model->getSelectedObjects();
}

/*!
 * \brief Fügt eine Komponente oder ein Textfeld ein.
 *
 * Wenn sich an der ausgewählten neuen Position keine Komponente oder Textfeld befindet, wird dieses dort eingefügt.
 */
void NetworkView::paste(void)
{
    _model->pasteGridObjects(_copiedObjects, _lastClickedPosition);
}

void NetworkView::enterEvent(QEvent* event)
{
    updateOverrideCursor();
    QWidget::enterEvent(event);
}

void NetworkView::leaveEvent(QEvent* event)
{
    QApplication::restoreOverrideCursor();
    QWidget::leaveEvent(event);

    deleteSampleObjectsAndHighlights();
}


void NetworkView::multiselect(QPointF endOfSelectionPosition, bool isEndOfSelection)
{
    QRectF selectionArea = QRectF(_firstPositionMultiselect, endOfSelectionPosition);
    int selectionAreaSize = selectionArea.height() * selectionArea.width();
    bool selectionAreaHasNoExpansion = (0 == selectionAreaSize);
    if (selectionAreaHasNoExpansion)
    {
        selectionArea.setSize(QSize(1, 1));
    }

    bool selectionAreaChanged = (_multiselectRect != nullptr && selectionArea != _multiselectRect->boundingRect());
    if (selectionAreaChanged || isEndOfSelection)
    {
        delete _multiselectRect;
        _multiselectRect = nullptr;
    }

    if (!isEndOfSelection && (selectionAreaChanged || nullptr == _multiselectRect))
    {
        _multiselectRect = _model->addRect(selectionArea);
    }

    bool controlIsPressed = QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier);
    if (!controlIsPressed)
    {
        _model->deselectAllItems();
    }
    _model->selectObjectsInArea(selectionArea);
    _model->update();
}

void NetworkView::updateOverrideCursor(void)
{
    QApplication::restoreOverrideCursor();
    if (_mouseMode == ConnectionMode)
    {
        QApplication::setOverrideCursor(Qt::CrossCursor);
    }
    else if (_mouseMode == SelectionMode)
    {
        if(_mouseIsPressed)
        {
            QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        }
        else
        {
            QApplication::setOverrideCursor(Qt::OpenHandCursor);
        }
    }
}

QGraphicsItem* NetworkView::deleteGraphicsItem(QGraphicsItem* graphicsItem)
{
    if (nullptr != graphicsItem)
    {
        _model->removeItem(graphicsItem);
        delete graphicsItem;
        graphicsItem = nullptr;

        _model->update();
    }
    return graphicsItem;
}

void NetworkView::startConnection(QPointF scenePosition)
{
    _connectionStartComponentPort = _model->getComponentPortAtPosition(scenePosition);
    bool foundComponentPort = _connectionStartComponentPort != nullptr;
    if (foundComponentPort)
    {
        deleteSampleObjectsAndHighlights();
        Component* foundComponent = _connectionStartComponentPort->getComponent();
        Component::Port port = _connectionStartComponentPort->getPort();

        int hitBoxHighlight = Component::_hitBoxSizeAtPort / 1.5;
        int positionX = foundComponent->getPortPosition(port).x() - hitBoxHighlight;
        int positionY = foundComponent->getPortPosition(port).y() - hitBoxHighlight;
        QColor color = QColor(255, 0, 0, 55);

        QGraphicsItem* highlightedRect = _model->addRect(positionX, positionY, 2 * hitBoxHighlight,
                                                         2 * hitBoxHighlight, Qt::NoPen,
                                                         color);
        _previousHighlightedPort = highlightedRect;

        Component* tempComponentForConnection = _connectionStartComponentPort->getComponent();
        _tempComponentListForConnections = _model->getComponents();
        _tempComponentListForConnections.removeOne(tempComponentForConnection);
    }
}

void NetworkView::startSelectionOrMove(QPointF scenePosition)
{
    GridObject* selectedObject = _model->getObjectAtPosition(scenePosition);
    bool isObjectAtPosition = (nullptr != selectedObject);
    if (isObjectAtPosition)
    {
        if (selectedObject->isSelected())
        {
            startMove(scenePosition);
        }
        else
        {
            startMultiSelection(scenePosition);
        }
    }
    else
    {
        startMultiSelection(scenePosition);
    }
}

void NetworkView::showSampleComponent(QPointF scenePosition, Component::ComponentType componentType)
{
    deleteSampleObjectsAndHighlights();
    QPointF gridPosition = _model->mapSceneToGrid(scenePosition);
    if (!_model->hasObjectAtPosition(scenePosition))
    {
        Component* sampleComponent;
        if (componentType == Component::ComponentType::Resistor)
        {
            sampleComponent = new Resistor(QString("R"), 0, gridPosition.toPoint().x(),
                                           gridPosition.toPoint().y(), _isVerticalComponentDefault, 0);
        }
        else
        {
            sampleComponent = new PowerSupply(QString("Q"), gridPosition.toPoint().x(),
                                              gridPosition.toPoint().y(), _isVerticalComponentDefault, 0,
                                              0);
        }
        _sampleComponent = sampleComponent;
        _model->addItem(_sampleComponent);
        highlightGrid(scenePosition, Defines::highlightColor);
    }
}

void NetworkView::showSampleComponent(QPointF scenePosition, NetworkView::MouseMode mouseMode)
{
    Component::ComponentType componentType = convertMouseModeToComponentType(mouseMode);
    showSampleComponent(scenePosition, componentType);
}

void NetworkView::connectionMoveEvent(QPointF scenePosition)
{
    ComponentPort* foundComponentPort = _model->getComponentPortAtPosition(scenePosition);
    if (foundComponentPort != nullptr)
    {
        if (!_mouseIsPressed)
        {
            highlightComponentPort(foundComponentPort, Defines::highlightColor);
        }
        else if (_mouseIsPressed && _connectionStartComponentPort != nullptr &&
                 foundComponentPort->getComponent() != _connectionStartComponentPort->getComponent())
        {
            QColor color = QColor(255, 0, 0, 55);
            highlightComponentPort(foundComponentPort, color);
        }
        _model->update();
    }
}

void NetworkView::highlightComponentPort(ComponentPort* componentPortToHighlight, QColor highlightColor)
{
    deleteSampleObjectsAndHighlights();

    Component* foundComponent = componentPortToHighlight->getComponent();
    Component::Port port = componentPortToHighlight->getPort();

    int hitBoxHighlight = Component::_hitBoxSizeAtPort / 1.5;
    int positionX = foundComponent->getPortPosition(port).x() - hitBoxHighlight;
    int positionY = foundComponent->getPortPosition(port).y() - hitBoxHighlight;

    if (_previousHighlightedRect == nullptr)
    {
        QGraphicsItem* highlightedRect = _model->addRect(positionX, positionY, 2 * hitBoxHighlight,
                                                         2 * hitBoxHighlight, Qt::NoPen,
                                                         highlightColor);
        _previousHighlightedRect = highlightedRect;
    }
}

void NetworkView::showSampleDescription(QPointF scenePosition)
{
    if (!_model->hasObjectAtPosition(scenePosition))
    {
        QPointF gridPosition = _model->mapSceneToGrid(scenePosition);
        _sampleDescription = new Description(gridPosition.x(), gridPosition.y(), 0);
        _model->addItem(_sampleDescription);
        highlightGrid(scenePosition, Defines::highlightColor);
    }
}

void NetworkView::startMultiSelection(QPointF scenePosition)
{
    _firstPositionMultiselect = scenePosition;
    bool controlIsPressed = QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier);
    if (!controlIsPressed)
    {
        _model->deselectAllItems();
    }
    _selectedObjectToMove = nullptr;
}

void NetworkView::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        emit changeToSelectionMode();
    }
}

bool NetworkView::isAllowedToChangeMode(void)
{
    bool startedMove = (nullptr != _selectedObjectToMove);
    bool startedConnection = (nullptr != _connectionStartComponentPort || nullptr != _previousHighlightedPort);
    bool isAllowedToChangeMode = !startedMove && !startedConnection;
    return isAllowedToChangeMode;
}

void NetworkView::selectAll()
{
    _model->selectAll();
}

void NetworkView::selectNothing()
{
    _model->deselectAllItems();
}

Component::ComponentType NetworkView::convertMouseModeToComponentType(NetworkView::MouseMode mouseMode)
{
    bool isResistorMode = mouseMode == MouseMode::ResistorMode;
    bool isPowerSupplyMode = mouseMode == MouseMode::PowerSupplyMode;
    if (isResistorMode)
    {
        return Component::ComponentType::Resistor;
    }
    else if (isPowerSupplyMode)
    {
        return Component::ComponentType::PowerSupply;
    }
    else
    {
        return Component::ComponentType::Null;
    }
}

void NetworkView::addInitialComponentWithMouseMode(QPointF scenePosition, NetworkView::MouseMode mouseMode)
{
    Component::ComponentType componentType = convertMouseModeToComponentType(mouseMode);
    Component* createdComponent = _model->addComponent(scenePosition, componentType,
                                                       _isVerticalComponentDefault);
    if (createdComponent != nullptr)
    {
        EditView* editView = new EditView(createdComponent, _model, true, this, nullptr);
        editView->show();
    }
}

void NetworkView::buildConnection(QPointF scenePositionOfEnd)
{
    bool connectionStarted = (nullptr != _connectionStartComponentPort);
    if (connectionStarted)
    {
        //TODO: entferne _tempComponentListForConnections
        ComponentPort* connectionComponentPortEnd = nullptr;
        for (Component* component : _tempComponentListForConnections)
        {
            bool hasFoundPort = component->hasPortAtPosition(scenePositionOfEnd);
            if (hasFoundPort)
            {
                Component::Port port = component->getPort(scenePositionOfEnd);
                connectionComponentPortEnd = new ComponentPort(component, port);
            }
        }

        bool foundComponentPortAtEnd = (connectionComponentPortEnd != nullptr);
        if (foundComponentPortAtEnd)
        {
            _model->addConnection(*_connectionStartComponentPort, *connectionComponentPortEnd);
        }

        delete _connectionStartComponentPort;
        _connectionStartComponentPort = nullptr;
    }
}

void NetworkView::deleteConnectionHighlights()
{
    _previousHighlightedPort = deleteGraphicsItem(_previousHighlightedPort);
    _tempComponentListForConnections.clear();
}

bool NetworkView::moveIfNeeded(QPointF scenePosition)
{
    bool hasStartedMoving = (_selectedObjectToMove != nullptr);
    if (hasStartedMoving)
    {
        if (!_model->hasObjectAtPosition(scenePosition))
        {
            if (!_selectedObjectToMove->boundingRect().contains(scenePosition))
            {
                _model->moveMultiselectObjects(_model->getSelectedObjects(), _selectedObjectToMove,
                                               scenePosition);
            }
        }
    }
    _selectedObjectToMove = nullptr;
    _model->update();
    return hasStartedMoving;
}

void NetworkView::addInitialDescription(QPointF scenePosition)
{
    bool ok;
    Description* createdDescription = _model->addDescriptionField(scenePosition, false);
    if (createdDescription != nullptr)
    {
        QString text = QInputDialog::getMultiLineText(this, "Textfeld bearbeiten", "Text bearbeiten", "",
                                                      &ok, Qt::WindowCloseButtonHint);

        if (ok && !text.isEmpty())
        {
            createdDescription->setText(text);
        }
    }
}

void NetworkView::startMove(QPointF scenePosition)
{
    _selectedObjectToMove = _model->getObjectAtPosition(scenePosition);
    updateOverrideCursor();
}

