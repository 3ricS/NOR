#include "defines.h"
#include "networkview.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QDebug>

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
    QPointF gridPosition = scenePositionToGrid(scenePosition);

    //Merken des zuletzt geklickten Bereichs, wird beim Paste benötigt
    _lastClickedPosition = gridPosition;

    if (_multiselectRect != nullptr)
    {
        delete _multiselectRect;
    }
    _multiselectRect = nullptr;

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
                Component* createdComponent = _model->addComponent(gridPosition, componentType,
                                                                   _isVerticalComponentDefault);
                if (createdComponent != nullptr)
                {
                    EditView* editView = new EditView(createdComponent, _model, true, this, nullptr);
                    editView->show();
                }
            }
            else if (Qt::RightButton == mouseEvent->button())
            {
                rotateComponent(scenePosition);
            }
        }
            break;
        case MouseMode::ConnectionMode:
        {
            bool connectionStarted = (nullptr != _connectionStartComponentPort);
            if (connectionStarted)
            {
                //TODO: entferne _tempComponentListForConnections
                ComponentPort* connectionComponentPortEnd = nullptr;
                for (Component* component : _tempComponentListForConnections)
                {
                    bool hasFoundPort = component->hasPortAtPosition(scenePosition);
                    if (hasFoundPort)
                    {
                        Component::Port port = component->getPort(scenePosition);
                        connectionComponentPortEnd = new ComponentPort(component, port);
                    }
                }

                if (connectionComponentPortEnd != nullptr)
                {
                    _model->addConnection(*_connectionStartComponentPort, *connectionComponentPortEnd);
                    _connectionStartComponentPort = nullptr;
                }
            }
            _tempComponentListForConnections.clear();
            if (nullptr != _previousHighlightedPort)
            {
                deleteSampleObjectsAndHighlights();
                _model->removeItem(_previousHighlightedPort);
                delete _previousHighlightedPort;
                _previousHighlightedPort = nullptr;

                _model->update();
            }
        }
            break;
        case MouseMode::SelectionMode:
        {
            QApplication::restoreOverrideCursor();
            //Move Event nur auslösen, wenn Objekt an neuer GridPosition
            if (!_model->hasObjectAtPosition(gridPosition))
            {
                if ((_selectedComponentToMove != nullptr && _selectedComponentToMove->getPosition() != gridPosition) ||
                    (_selectedDescriptionToMove != nullptr &&
                     QPointF(_selectedDescriptionToMove->getXPosition(), _selectedDescriptionToMove->getYPosition()) !=
                     gridPosition))
                {
                    int previousComoponentXPosition = 0;
                    int previousComoponentYPosition = 0;
                    int diffXAfterMoving = 0;
                    int diffYAfterMoving = 0;

                    if (_selectedComponentToMove != nullptr)
                    {
                        previousComoponentXPosition = _selectedComponentToMove->getXPosition();
                        previousComoponentYPosition = _selectedComponentToMove->getYPosition();
                    }
                    else if (_selectedDescriptionToMove != nullptr)
                    {
                        previousComoponentXPosition = _selectedDescriptionToMove->getXPosition();
                        previousComoponentYPosition = _selectedDescriptionToMove->getYPosition();
                    }

                    _model->moveComponent(_selectedComponentToMove, _selectedDescriptionToMove, gridPosition);

                    if (_selectedComponentToMove != nullptr)
                    {
                        diffXAfterMoving = _selectedComponentToMove->getXPosition() - previousComoponentXPosition;
                        diffYAfterMoving = _selectedComponentToMove->getYPosition() - previousComoponentYPosition;
                    }
                    else if (_selectedDescriptionToMove != nullptr)
                    {
                        diffXAfterMoving = _selectedDescriptionToMove->getXPosition() - previousComoponentXPosition;
                        diffYAfterMoving = _selectedDescriptionToMove->getYPosition() - previousComoponentYPosition;
                    }

                    _model->moveMultiselectComponents(_model->getComponents(), _model->getDescriptions(),
                                                      _selectedComponentToMove, _selectedDescriptionToMove,
                                                      diffXAfterMoving, diffYAfterMoving);
                }
            }
            //Multiselection der Bauteile
            multiselect(scenePosition, true);

            _selectedComponentToMove = nullptr;
            _selectedDescriptionToMove = nullptr;

            _model->selectObjectsAtPosition(scenePosition);
            _model->update();
        }
            break;
        case MouseMode::DescriptionMode:
        {
            if (mouseEvent->button() == Qt::LeftButton)
            {
                bool ok;
                Description* createdDescription = _model->addDescriptionField(gridPosition, false);
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
    _componentOrDescriptionIsGrabbed = _model->hasObjectAtPosition(gridPosition);

    //Keine Verbindung begonnen: _connectionStartComponentPort muss auf Nullptr zeigen
    _connectionStartComponentPort = nullptr;
    _selectedComponentToMove = nullptr;
    _selectedDescriptionToMove = nullptr;

    if(ConnectionMode == _mouseMode)
    {
        startConnection(scenePosition);
    }
    else if(SelectionMode == _mouseMode)
    {
        startSelection(scenePosition);
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
            if (_mouseIsPressed && _selectedComponentToMove == nullptr && _selectedDescriptionToMove == nullptr)
            {
                multiselect(scenePosition, false);
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
        editNetworkOrDescription();
    }
}

QPointF NetworkView::scenePositionToGrid(QPointF scenePosition)
{
    //TODO: diese Funktion ist in NetworkGraphics; diese muss gelöscht werden
    qreal xPos = scenePosition.toPoint().x() / Defines::gridLength * Defines::gridLength - (Defines::gridLength / 2);
    qreal yPos = scenePosition.toPoint().y() / Defines::gridLength * Defines::gridLength - (Defines::gridLength / 2);
    return QPointF(xPos, yPos);
}

void NetworkView::deleteSampleObjectsAndHighlights(void)
{
    _previousHighlightedRect = deleteGraphicsItem(_previousHighlightedRect);
    _sampleComponentOnMoveEvent = dynamic_cast<Component*>(deleteGraphicsItem(_sampleComponentOnMoveEvent));
    _sampleDescriptionOnMoveEvent = dynamic_cast<Description*>(deleteGraphicsItem(_sampleDescriptionOnMoveEvent));
}

void NetworkView::highlightRect(QPointF scenePosition, QColor highlightColor)
{
    QPointF gridPositionOne = scenePositionToGrid(scenePosition);
    int positionX = gridPositionOne.toPoint().x();
    int positionY = gridPositionOne.toPoint().y();

    if (!_model->hasObjectAtPosition(gridPositionOne))
    {
        QGraphicsItem* highlightedRect = _model->addRect(positionX - (Defines::gridLength / 2),
                                                         positionY - (Defines::gridLength / 2),
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
void NetworkView::deleteSelectedItem(void)
{
    //TODO: nach Model verschieben
    //TODO: auf Undostack
    for (Component* component : _model->getComponents())
    {
        if (component->isSelected())
        {
            if (_copiedComponents.contains(component))
            {
                _copiedComponents.removeOne(component);
            }
            _model->deleteComponent(component);
        }
    }

    for (Description* description : _model->getDescriptions())
    {
        if (description->isSelected())
        {
            if (_copiedDescriptions.contains(description))
            {
                _copiedDescriptions.removeOne(description);
            }
            _model->deleteDescription(description);
        }
    }

    for (Connection* connection : _model->getConnections())
    {
        if (connection->isSelected())
        {
            _model->deleteConnection(connection);
        }
    }
    _model->deselectAllItems();
}

/*!
 * \brief Bearbeiten des Netzwerkes oder eines Textfeldes.
 *
 * Es werden entweder eine Komponente oder ein Textfeld in dem Netzwerk bearbeitet.
 */
void NetworkView::editNetworkOrDescription(void)
{
    QList<Component*> selectedComponents = _model->getSelectedComponents();
    QList<Description*> selectedDescriptionFields = _model->getSelectedDescriptionFields();

    for (Component* component : selectedComponents)
    {
        EditView* editView = new EditView(component, _model, false, this, _model->getUndoStack());
        editView->show();
    }

    for (Description* description : selectedDescriptionFields)
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

bool NetworkView::lookingForFreeSpaceToDuplicate(int xPos, int yPos, int& xWaytoTheRight)
{
    bool foundUnusedSpace = false;
    while (true)
    {
        if (!_model->hasObjectAtPosition(
                QPointF(xPos + xWaytoTheRight, yPos)))
        {
            foundUnusedSpace = true;
            return foundUnusedSpace;
        }

        xWaytoTheRight += Defines::gridLength;
    }
}

//Sucht nach der DurchschnittsXPosition und der DurchschnittsYPosition
QPointF NetworkView::findScrollPosition()
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
                averageX += c->getXPosition();
                averageY += c->getYPosition();

                anzahlResistorAndDescriptions++;
            }
        }
    }
    if (_model->getDescriptions().count() != 0)
    {
        for (Description* c : _model->getDescriptions())
        {
            averageX += c->getXPosition();
            averageY += c->getYPosition();
            anzahlResistorAndDescriptions++;
        }
    }
    if (anzahlResistorAndDescriptions != 0)
    {
        averageY /= anzahlResistorAndDescriptions;
        averageX /= anzahlResistorAndDescriptions;
    }
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
    _copiedComponents = _model->findSelectedComponents();
    _copiedDescriptions = _model->findSelectedDescription();
    if (_copiedComponents.count() != 0)
    {
        _model->cutComponent(_copiedComponents);
    }
    if (_copiedDescriptions.count() != 0)
    {
        _model->cutDescription(_copiedDescriptions);
    }
}

void NetworkView::setMouseMode(NetworkView::MouseMode newMode)
{
    _mouseMode = newMode;

    //Entfernen der Beispiel Objekte
    deleteSampleObjectsAndHighlights();

    //Angezeigte Cursor ändern
    changeOverrideCursor();
}

/*!
 * \brief Dupliziert eine Komponente.
 *
 * Die ausgewählte Komponente wird in ein freies Feld nach rechts dupliziert.
 */
void NetworkView::duplicate(void)
{
    for (Component* component : _model->getComponents())
    {
        if (component->isSelected())
        {
            int xWayToTheRight = Defines::gridLength;
            if (lookingForFreeSpaceToDuplicate(component->getXPosition(), component->getYPosition(), xWayToTheRight))
            {
                _model->duplicateComponent(component, component->getXPosition() + xWayToTheRight,
                                           component->getYPosition());
            }
        }
    }

    for (Description* description : _model->getDescriptions())
    {
        if (description->isSelected())
        {
            int xWayToTheRight = Defines::gridLength;
            if (lookingForFreeSpaceToDuplicate(description->getXPosition(), description->getYPosition(),
                                               xWayToTheRight))
            {
                _model->duplicateDescription(description, description->getXPosition() + xWayToTheRight,
                                             description->getYPosition());
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
    //Zuerst alle alten Einträge löschen
    _copiedComponents.clear();
    _copiedDescriptions.clear();

    _copiedComponents = _model->findSelectedComponents();
    _copiedDescriptions = _model->findSelectedDescription();
}

/*!
 * \brief Fügt eine Komponente oder ein Textfeld ein.
 *
 * Wenn sich an der ausgewählten neuen Position keine Komponente oder Textfeld befindet, wird dieses dort eingefügt.
 */
void NetworkView::paste(void)
{
    Component* firstComp = nullptr;
    if (!_copiedComponents.empty())
    {
        firstComp = _copiedComponents.first();
    }

    int xSpace = 0;
    int ySpace = 0;
    int i = 1;
    for (Component* component : _copiedComponents)
    {
        QPointF point = _model->mapSceneToGrid(
                QPointF(_lastClickedPosition.x() - xSpace, _lastClickedPosition.y() - ySpace));
        if (!_model->hasObjectAtPosition(point))
        {
            _model->duplicateComponent(component, _lastClickedPosition.x() - xSpace, _lastClickedPosition.y() - ySpace);
            calculateDistanceToNextComponent(i, firstComp, xSpace, ySpace);
        }

    }
    //Zurücksetzten der Laufvariablen
    i = 1;
    Description* firstDescription = nullptr;
    if (!_copiedDescriptions.empty())
    {
        firstDescription = _copiedDescriptions.first();
    }
    if (firstComp != nullptr && firstDescription != nullptr)
    {
        xSpace = firstComp->getXPosition() - firstDescription->getXPosition();
        ySpace = firstComp->getYPosition() - firstDescription->getYPosition();
    }
    for (Description* description : _copiedDescriptions)
    {
        if (!_model->hasObjectAtPosition(
                scenePositionToGrid(QPointF(_lastClickedPosition.x() - xSpace, _lastClickedPosition.y() - ySpace))))
        {
            _model->duplicateDescription(description, _lastClickedPosition.x() - xSpace,
                                         _lastClickedPosition.y() - ySpace);
            calculateDistanceToNextDescription(i, firstComp, xSpace, ySpace);
        }

    }
}

void NetworkView::enterEvent(QEvent* event)
{
    changeOverrideCursor();
    QWidget::enterEvent(event);
}

void NetworkView::leaveEvent(QEvent* event)
{
    QApplication::restoreOverrideCursor();
    QWidget::leaveEvent(event);

    deleteSampleObjectsAndHighlights();
}

void NetworkView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        QApplication::restoreOverrideCursor();
        _model->deselectAllItems();
        deleteSampleObjectsAndHighlights();
    }
    QGraphicsView::keyPressEvent(event);
}

void NetworkView::multiselect(QPointF endOfSelectionPosition, bool isEndOfSelection)
{
    QRectF selectionArea = QRectF(_firstPositionMultiselect, endOfSelectionPosition);
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

    _model->selectObjectsInArea(selectionArea);
    _model->update();
}

void NetworkView::changeOverrideCursor()
{
    QApplication::restoreOverrideCursor();
    if (_mouseMode == ConnectionMode)
    {
        QApplication::setOverrideCursor(Qt::CrossCursor);
    }
}

QList<Component*> NetworkView::findSelectedComponent()
{
    QList<Component*> componentList;
    for (Component* component : _model->getComponents())
    {
        if (component->isSelected())
        {
            componentList.append(component);
        }
    }
    return componentList;
}

QList<Description*> NetworkView::findSelectedDescription()
{
    QList<Description*> descriptionList;
    for (Description* description : _model->getDescriptions())
    {
        if (description->isSelected())
        {
            descriptionList.append(description);
        }
    }
    return descriptionList;
}

void NetworkView::calculateDistanceToNextComponent(int& i, Component* firstComp, int& xSpace, int& ySpace)
{
    if (i < _copiedComponents.count() && firstComp != nullptr)
    {
        xSpace = firstComp->getXPosition() - _copiedComponents[i]->getXPosition();
        ySpace = firstComp->getYPosition() - _copiedComponents[i]->getYPosition();
    }
    else if (i == _copiedComponents.count())
    {
        xSpace = 0;
        ySpace = 0;
    }
    i++;
}

void NetworkView::calculateDistanceToNextDescription(int& i, Component* firstComp, int& xSpace, int& ySpace)
{
    if (i < _copiedDescriptions.count() && firstComp != nullptr)
    {
        xSpace = firstComp->getXPosition() - _copiedDescriptions[i]->getXPosition();
        ySpace = firstComp->getYPosition() - _copiedDescriptions[i]->getYPosition();
    }
    i++;
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

        int hitBoxHighlight = Component::_hitBoxSize / 1.5;
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

void NetworkView::startSelection(QPointF scenePosition)
{
    _model->deselectAllItems();
    _firstPositionMultiselect = scenePosition;
    bool isObjectAtPosition = _model->hasObjectAtPosition(scenePosition);
    if (isObjectAtPosition)
    {
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        _selectedComponentToMove = _model->getComponentAtPosition(scenePosition);
        if (nullptr == _selectedComponentToMove)
        {
            _selectedDescriptionToMove = _model->getDescriptionAtPosition(scenePosition);
        }
    }
    else
    {
        _selectedComponentToMove = nullptr;
        _selectedDescriptionToMove = nullptr;
    }
}

void NetworkView::showSampleComponent(QPointF scenePosition, Component::ComponentType componentType)
{
    deleteSampleObjectsAndHighlights();
    QPointF gridPosition = _model->mapSceneToGrid(scenePosition);
    if (!_model->hasObjectAtPosition(scenePosition))
    {
        Component* sampleResistor;
        if (componentType == Component::ComponentType::Resistor)
        {
            sampleResistor = new Resistor(QString("R"), 0, gridPosition.toPoint().x(),
                                          gridPosition.toPoint().y(), _isVerticalComponentDefault, 0);
        }
        else
        {
            sampleResistor = new PowerSupply(QString("Q"), gridPosition.toPoint().x(),
                                             gridPosition.toPoint().y(), _isVerticalComponentDefault, 0,
                                             0);
        }
        _sampleComponentOnMoveEvent = sampleResistor;
        _model->addItem(_sampleComponentOnMoveEvent);
        highlightRect(scenePosition, Defines::highlightColor);
    }
}

void NetworkView::showSampleComponent(QPointF scenePosition, const NetworkView::MouseMode mouseMode)
{
    Component::ComponentType componentType;
    bool isResistorMode = mouseMode == MouseMode::ResistorMode;
    bool isPowerSupplyMode = mouseMode == MouseMode::PowerSupplyMode;
    if (isResistorMode)
    {
        componentType = Component::ComponentType::Resistor;
    }
    else if (isPowerSupplyMode)
    {
        componentType = Component::ComponentType::PowerSupply;
    }
    if (isResistorMode || isPowerSupplyMode)
    {
        showSampleComponent(scenePosition, componentType);
    }
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

    int hitBoxHighlight = Component::_hitBoxSize / 1.5;
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
        _sampleDescriptionOnMoveEvent = new Description(gridPosition.x(), gridPosition.y(), 0);
        _model->addItem(_sampleDescriptionOnMoveEvent);
        highlightRect(scenePosition, Defines::highlightColor);
    }
}

