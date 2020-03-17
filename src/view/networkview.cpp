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
 * \param   model
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

    if(_multiselectRect != nullptr)
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
            if(Qt::LeftButton == mouseEvent->button())
            {
                Component* createdComponent = _model->addComponent(gridPosition, componentType,
                                                                   _isVerticalComponentDefault);
                if(createdComponent != nullptr)
                {
                    EditView* editView = new EditView(createdComponent, _model, true, this, nullptr);
                    editView->show();
                }
            }
            else if(Qt::RightButton == mouseEvent->button())
            {
                rotateComponent(gridPosition, scenePosition);
            }
        }
            break;
        case MouseMode::ConnectionMode:
        {
            bool connectionStarted = (nullptr != _connectionStartComponentPort);
            if(connectionStarted)
            {
                ComponentPort* connectionComponentPortEnd = nullptr;
                for (Component* component : _tempComponentListForConnections)
                {
                    bool hasFoundPort = component->hasPortAtPosition(scenePosition);
                    if(hasFoundPort)
                    {
                        Component::Port port = component->getPort(scenePosition);
                        connectionComponentPortEnd = new ComponentPort(component, port);
                    }
                }

                if(connectionComponentPortEnd != nullptr)
                {
                    _model->addConnection(*_connectionStartComponentPort, *connectionComponentPortEnd);
                    _connectionStartComponentPort = nullptr;
                }
            }
            _tempComponentListForConnections.clear();
            if(nullptr != _previousHighlightedPort)
            {
                deleteSampleObjects();
                _model->removeItem(_previousHighlightedPort);
                delete _previousHighlightedPort;
                _previousHighlightedPort = nullptr;

                _model->update();
            }
        }
            break;
        case MouseMode::SelectionMode:
        {
            //Move Event nur auslösen, wenn Objekt an neuer GridPosition
            if(!_model->isThereAComponentOrADescription(gridPosition))
            {
                if((_selectedComponentToMove != nullptr && _selectedComponentToMove->getPosition() != gridPosition) ||
                   (_selectedDescriptionToMove != nullptr &&
                    QPointF(_selectedDescriptionToMove->getXPosition(), _selectedDescriptionToMove->getYPosition()) !=
                    gridPosition))
                {
                    int previousComoponentXPosition = 0;
                    int previousComoponentYPosition = 0;
                    int diffXAfterMoving = 0;
                    int diffYAfterMoving = 0;

                    if((_selectedComponentToMove != nullptr) || (_selectedDescriptionToMove != nullptr))
                    {
                        previousComoponentXPosition = _selectedDescriptionToMove->getXPosition();
                        previousComoponentYPosition = _selectedDescriptionToMove->getYPosition();
                    }

                    _model->moveComponent(_selectedComponentToMove, _selectedDescriptionToMove, gridPosition);

                    if((_selectedComponentToMove != nullptr) || (_selectedDescriptionToMove != nullptr))
                    {
                        diffXAfterMoving = _selectedDescriptionToMove->getXPosition() - previousComoponentXPosition;
                        diffYAfterMoving = _selectedDescriptionToMove->getYPosition() - previousComoponentYPosition;
                    }

                    _model->moveMultiselectComponents(_model->getComponents(), _model->getDescriptions(),
                                                      _selectedComponentToMove, _selectedDescriptionToMove,
                                                      diffXAfterMoving, diffYAfterMoving);
                }
            }
            if(!_isMoved)
            {
                _model->deselectAllItems();
            }
            //Multiselection der Bauteile
            if((_firstPositionMultiselect != gridPosition) &&
               (!_model->isThereAComponentOrADescription(gridPosition)))
            {
                _lastPositionMultiselect = scenePosition;
                multiselecting();
                delete _multiselectRect;
                _multiselectRect = nullptr;
            }

            QApplication::setOverrideCursor(Qt::OpenHandCursor);
            _selectedComponentToMove = nullptr;
            _selectedDescriptionToMove = nullptr;

            _model->selectObjectsAtPosition(scenePosition);
            _model->update();
        }
            break;
        case MouseMode::DescriptionMode:
        {
            if(mouseEvent->button() == Qt::LeftButton)
            {
                bool ok;
                DescriptionField* createdDescription = _model->addDescriptionField(gridPosition, false);
                if(createdDescription != nullptr)
                {
                    QString text = QInputDialog::getMultiLineText(this, "Textfeld bearbeiten", "Text bearbeiten", "",
                                                                  &ok, Qt::WindowCloseButtonHint);

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
    _selectedDescriptionToMove = nullptr;

    switch (_mouseMode)
    {
        case ConnectionMode:
        {
            _connectionStartComponentPort = _model->getComponentPortAtPosition(scenePosition);
            if(_connectionStartComponentPort != nullptr)
            {
                deleteSampleObjects();
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
            break;
        case SelectionMode:
        {
            if(!_isMoved)
            {
                _model->deselectAllItems();
            }
            _isMoved = false;
            bool isComponentAtPosition = _model->isThereAComponentOrADescription(gridPosition);
            if(isComponentAtPosition)
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
                _firstPositionMultiselect = scenePosition;
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
    deleteSampleObjects();

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
                                                            gridPosition.toPoint().y(), _isVerticalComponentDefault, 0,
                                                            0);
                _sampleComponentOnMoveEvent = sampleResistor;
                _model->addItem(_sampleComponentOnMoveEvent);
                highlightRect(scenePosition, _highlightColor);
            }
        }
            break;
        case ConnectionMode:
        {
            ComponentPort* foundComponentPort = _model->getComponentPortAtPosition(scenePosition);
            if(foundComponentPort != nullptr)
            {
                Component* foundComponent = foundComponentPort->getComponent();
                Component::Port port = foundComponentPort->getPort();

                int hitBoxHighlight = Component::_hitBoxSize / 1.5;
                int positionX = foundComponent->getPortPosition(port).x() - hitBoxHighlight;
                int positionY = foundComponent->getPortPosition(port).y() - hitBoxHighlight;

                if(!_mouseIsPressed)
                {
                    QGraphicsItem* highlightedRect = _model->addRect(positionX, positionY, 2 * hitBoxHighlight,
                                                                     2 * hitBoxHighlight, Qt::NoPen,
                                                                     _highlightColor);
                    _previousHighlightedRect = highlightedRect;
                }
                else if(_mouseIsPressed && _connectionStartComponentPort != nullptr &&
                        foundComponentPort->getComponent() != _connectionStartComponentPort->getComponent())
                {
                    deleteSampleObjects();
                    QColor color = QColor(255, 0, 0, 55);
                    QGraphicsItem* highlightedRect = _model->addRect(positionX, positionY, 2 * hitBoxHighlight,
                                                                     2 * hitBoxHighlight, Qt::NoPen,
                                                                     color);
                    _previousHighlightedRect = highlightedRect;
                }
                _model->update();
            }
        }
            break;
        case SelectionMode:
        {
            if(_mouseIsPressed && _componentOrDescriptionIsGrabbed)
            {
                _isMoved = true;
                highlightRect(scenePosition, _highlightColor);
            }
            if(_mouseIsPressed && _selectedComponentToMove == nullptr && _selectedDescriptionToMove == nullptr)
            {
                _isMoved = true;
                _lastPositionMultiselect = scenePosition;
                multiselecting();
            }
        }
            break;
        case DescriptionMode:
        {
            if(!_model->isThereAComponentOrADescription(gridPosition))
            {
                _sampleDescriptionOnMoveEvent = new DescriptionField(gridPosition.x(), gridPosition.y(), 0);
                _model->addItem(_sampleDescriptionOnMoveEvent);
                highlightRect(scenePosition, _highlightColor);
            }
        }
            break;
    }
}

void NetworkView::mouseDoubleClickEvent([[maybe_unused]]QMouseEvent* event)
{
    if(_mouseMode == SelectionMode)
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

void NetworkView::deleteSampleObjects(void)
{
    _previousHighlightedRect = deleteGraphicsItem(_previousHighlightedRect);
    _sampleComponentOnMoveEvent = dynamic_cast<Component*>(deleteGraphicsItem(_sampleComponentOnMoveEvent));
    _sampleDescriptionOnMoveEvent = dynamic_cast<DescriptionField*>(deleteGraphicsItem(_sampleDescriptionOnMoveEvent));
}

void NetworkView::highlightRect(QPointF scenePosition, QColor highlightColor)
{
    QPointF gridPositionOne = scenePositionToGrid(scenePosition);
    int positionX = gridPositionOne.toPoint().x();
    int positionY = gridPositionOne.toPoint().y();

    if(!_model->isThereAComponentOrADescription(gridPositionOne))
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
        if(component->isSelected())
        {
            if(_copiedComponents.contains(component))
            {
                _copiedComponents.removeOne(component);
            }
            _model->deleteComponent(component);
        }
    }

    for (DescriptionField* description : _model->getDescriptions())
    {
        if(description->isSelected())
        {
            if(_copiedDescriptions.contains(description))
            {
                _copiedDescriptions.removeOne(description);
            }
            _model->deleteDescription(description);
        }
    }

    for (Connection* connection : _model->getConnections())
    {
        if(connection->isSelected())
        {
            _model->deleteConnection(connection);
        }
    }
    _model->deselectAllItems();
}

/*!
 * \brief Bearbeiten des Netzwerkes oder eines Textfeldes.
 *
 *
 */
void NetworkView::editNetworkOrDescription(void)
{
    QList<Component*> selectedComponents = _model->getSelectedComponents();
    QList<DescriptionField*> selectedDescriptionFields = _model->getSelectedDescriptionFields();

    for (Component* component : selectedComponents)
    {
        EditView* editView = new EditView(component, _model, false, this, _model->getUndoStack());
        editView->show();
    }

    for (DescriptionField* description : selectedDescriptionFields)
    {
        bool ok = false;
        QString text = QInputDialog::getMultiLineText(this, "Textfeld bearbeiten", "Text eingeben:",
                                                      description->getText(), &ok, Qt::WindowCloseButtonHint);
        if(ok)
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

void NetworkView::rotateComponent(QPointF gridPosition, QPointF scenePosition)
{
    if(MouseMode::SelectionMode != _mouseMode)
    {
        _isVerticalComponentDefault = !_isVerticalComponentDefault;
        deleteSampleObjects();
        if(!_model->isThereAComponentOrADescription(gridPosition))
        {
            if(MouseMode::PowerSupplyMode == _mouseMode)
            {
                Component* sampleResistor = new PowerSupply(QString("Q"), gridPosition.toPoint().x(),
                                                            gridPosition.toPoint().y(), _isVerticalComponentDefault, 0,
                                                            0);
                _sampleComponentOnMoveEvent = sampleResistor;
            }
            else if(MouseMode::ResistorMode == _mouseMode)
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
    else if(MouseMode::SelectionMode == _mouseMode)
    {
        for (Component* c : _model->getComponents())
        {
            if(c->isSelected())
            {
                _model->turnComponentRight(c);
            }
        }
    }
}

bool NetworkView::lookingForFreeSpaceToDuplicate(int xPos, int yPos, int &xWaytoTheRight)
{
    bool foundUnusedSpace = false;
    while (true)
    {
        if(!_model->isThereAComponentOrADescription(
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
    if(_model->getComponents().count() != 0)
    {
        for (Component* c : _model->getComponents())
        {
            if(c->getComponentTypeInt() == Component::Resistor)
            {
                averageX += c->getXPosition();
                averageY += c->getYPosition();

                anzahlResistorAndDescriptions++;
            }
        }
    }
    if(_model->getDescriptions().count() != 0)
    {
        for (DescriptionField* c : _model->getDescriptions())
        {
            averageX += c->getXPosition();
            averageY += c->getYPosition();
            anzahlResistorAndDescriptions++;
        }
    }
    if(anzahlResistorAndDescriptions != 0)
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
 *
 */
void NetworkView::rotateComponentByShortcut(void)
{
    QPointF gridPosition = scenePositionToGrid(_actualMoveScenePosition);
    rotateComponent(gridPosition, _actualMoveScenePosition);
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
    if(dlg.exec() == QDialog::Accepted)
    {
        //Zuerst fokussieren
        focusForPrint();

        //Alle Objekte im View werden gezeichnet
        QPainter p(&printer);
        render(&p);

        //Name der Schaltung
        QString name;
        if(_model->getFileName() == "")
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

void NetworkView::cut()
{
    _copiedComponents = _model->findSelectedComponent();
    _copiedDescriptions = _model->findSelectedDescription();
    if(_copiedComponents.count() != 0)
    {
        _model->cutComponent(_copiedComponents);
    }
    if(_copiedDescriptions.count() != 0)
    {
        _model->cutDescription(_copiedDescriptions);
    }
}

void NetworkView::setMouseMode(NetworkView::MouseMode newMode)
{
    _mouseMode = newMode;

    //Entfernen der Beispiel Objekte
    deleteSampleObjects();

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
        if(component->isSelected())
        {
            int xWayToTheRight = Defines::gridLength;
            if(lookingForFreeSpaceToDuplicate(component->getXPosition(), component->getYPosition(), xWayToTheRight))
            {
                _model->duplicateComponent(component, component->getXPosition() + xWayToTheRight,
                                           component->getYPosition());
            }
        }
    }

    for (DescriptionField* description : _model->getDescriptions())
    {
        if(description->isSelected())
        {
            int xWayToTheRight = Defines::gridLength;
            if(lookingForFreeSpaceToDuplicate(description->getXPosition(), description->getYPosition(), xWayToTheRight))
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

    _copiedComponents = _model->findSelectedComponent();
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
    if(!_copiedComponents.empty())
    {
        firstComp = _copiedComponents.first();
    }

    int xSpace = 0;
    int ySpace = 0;
    int i = 1;
    for (Component* component : _copiedComponents)
    {
        if(!_model->isThereAComponentOrADescription(
                scenePositionToGrid(QPointF(_lastClickedPosition.x() - xSpace, _lastClickedPosition.y() - ySpace))))
        {
            _model->duplicateComponent(component, _lastClickedPosition.x() - xSpace, _lastClickedPosition.y() - ySpace);
            calculateDistanceToNextComponent(i, firstComp, xSpace, ySpace);
        }

    }
    //Zurücksetzten der Laufvariablen
    i = 1;
    DescriptionField* firstDescription = nullptr;
    if(!_copiedDescriptions.empty())
    {
        qDebug() << "Ja";
        firstDescription = _copiedDescriptions.first();
    }
    if(firstComp != nullptr && firstDescription != nullptr)
    {
        xSpace = firstComp->getXPosition() - firstDescription->getXPosition();
        ySpace = firstComp->getYPosition() - firstDescription->getYPosition();
        qDebug() << xSpace;
        qDebug() << ySpace;
    }
    for (DescriptionField* description : _copiedDescriptions)
    {
        if(!_model->isThereAComponentOrADescription(
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
    QApplication::setOverrideCursor(Qt::CustomCursor);
    QWidget::leaveEvent(event);

    deleteSampleObjects();
}

void NetworkView::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
    {
        setMouseMode(NetworkView::MouseMode::SelectionMode);
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        _model->deselectAllItems();
        deleteSampleObjects();

        _model->deselectAllItems();
    }
    QGraphicsView::keyPressEvent(event);
}

void NetworkView::multiselecting(void)
{
    int firstClickedXPosition = _firstPositionMultiselect.toPoint().x();
    int firstClickedYPosition = _firstPositionMultiselect.toPoint().y();
    int lastClickedXPosition = _lastPositionMultiselect.toPoint().x();
    int lastClickedYPosition = _lastPositionMultiselect.toPoint().y();

    if((lastClickedXPosition > firstClickedXPosition && lastClickedYPosition > firstClickedXPosition)
       or (firstClickedXPosition > lastClickedXPosition && lastClickedYPosition > firstClickedYPosition)
       or (lastClickedXPosition > firstClickedXPosition && firstClickedYPosition > lastClickedYPosition)
       or (firstClickedXPosition > lastClickedXPosition && firstClickedYPosition > lastClickedYPosition))
    {
        delete _multiselectRect;
        _multiselectRect = _model->addRect(lastClickedXPosition, lastClickedYPosition,
                                           firstClickedXPosition - lastClickedXPosition,
                                           firstClickedYPosition - lastClickedYPosition);
    }

    for (Component* component : _model->getComponents())
    {
        component->setIsSelected(true);
        highlightRect(component->getPosition(), _highlightColor);

        if(((component->getXPosition() >= firstClickedXPosition) &&
            (component->getXPosition() <= lastClickedXPosition) &&
            (component->getYPosition() >= firstClickedYPosition) &&
            (component->getYPosition() <= lastClickedYPosition))
           or ((component->getXPosition() <= firstClickedXPosition) &&
               (component->getXPosition() >= lastClickedXPosition)
               && (component->getYPosition() <= firstClickedYPosition) &&
               (component->getYPosition() >= lastClickedYPosition))
           or ((component->getXPosition() >= firstClickedXPosition) &&
               (component->getXPosition() <= lastClickedXPosition)
               && (component->getYPosition() <= firstClickedYPosition) &&
               (component->getYPosition() >= lastClickedYPosition))
           or ((component->getXPosition() <= firstClickedXPosition) &&
               (component->getXPosition() >= lastClickedXPosition) &&
               (component->getYPosition() >= firstClickedYPosition) &&
               (component->getYPosition() <= lastClickedYPosition)))
        {
            component->setIsSelected(true);
            highlightRect(component->getPosition(), _highlightColor);
        }
        else
        {
            component->setIsSelected(false);
        }
    }
    for (DescriptionField* description : _model->getDescriptions())
    {
        if(((description->getXPosition() >= firstClickedXPosition) &&
            (description->getXPosition() <= lastClickedXPosition)
            && (description->getYPosition() >= firstClickedYPosition) &&
            (description->getYPosition() <= lastClickedYPosition))
           or ((description->getXPosition() <= firstClickedXPosition) &&
               (description->getXPosition() >= lastClickedXPosition)
               && (description->getYPosition() <= firstClickedYPosition) &&
               (description->getYPosition() >= lastClickedYPosition))
           or ((description->getXPosition() >= firstClickedXPosition) &&
               (description->getXPosition() <= lastClickedXPosition)
               && (description->getYPosition() <= firstClickedYPosition) &&
               (description->getYPosition() >= lastClickedYPosition))
           or ((description->getXPosition() <= firstClickedXPosition) &&
               (description->getXPosition() >= lastClickedXPosition)
               && (description->getYPosition() >= firstClickedYPosition) &&
               (description->getYPosition() <= lastClickedYPosition)))
        {
            description->setIsSelected(true);
            highlightRect(description->getPosition(), _highlightColor);
        }
        else
        {
            description->setIsSelected(false);
        }
    }
    _model->update();
}

void NetworkView::changeOverrideCursor()
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
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        }
            break;
        case ResistorMode:
        {
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        }
            break;
        case DescriptionMode:
        {
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        }
            break;
    }
}

QList<Component*> NetworkView::findSelectedComponent()
{
    QList<Component*> componentList;
    for (Component* component : _model->getComponents())
    {
        if(component->isSelected())
        {
            componentList.append(component);
        }
    }
    return componentList;
}

QList<DescriptionField*> NetworkView::findSelectedDescription()
{
    QList<DescriptionField*> descriptionList;
    for (DescriptionField* description : _model->getDescriptions())
    {
        if(description->isSelected())
        {
            descriptionList.append(description);
        }
    }
    return descriptionList;
}

void NetworkView::calculateDistanceToNextComponent(int &i, Component* firstComp, int &xSpace, int &ySpace)
{
    if(i < _copiedComponents.count() && firstComp != nullptr)
    {
        xSpace = firstComp->getXPosition() - _copiedComponents[i]->getXPosition();
        ySpace = firstComp->getYPosition() - _copiedComponents[i]->getYPosition();
    }
    else if(i == _copiedComponents.count())
    {
        xSpace = 0;
        ySpace = 0;
    }
    i++;
}

void NetworkView::calculateDistanceToNextDescription(int &i, Component* firstComp, int &xSpace, int &ySpace)
{
    if(i < _copiedDescriptions.count() && firstComp != nullptr)
    {
        xSpace = firstComp->getXPosition() - _copiedDescriptions[i]->getXPosition();
        ySpace = firstComp->getYPosition() - _copiedDescriptions[i]->getYPosition();
    }
    i++;
}

QGraphicsItem* NetworkView::deleteGraphicsItem(QGraphicsItem* graphicsItem)
{
    if(nullptr != graphicsItem)
    {
        _model->removeItem(graphicsItem);
        delete graphicsItem;
        graphicsItem = nullptr;

        _model->update();
    }
    return graphicsItem;
}
