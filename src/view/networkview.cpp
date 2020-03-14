#include "defines.h"
#include "networkview.h"

#include <QPrintDialog>
#include <QPrinter>


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
                gridDisappears();
                _model->removeItem(_previousHighlightedPort);
                delete _previousHighlightedPort;
                _previousHighlightedPort = nullptr;

                _model->update();
            }
        }
            break;
        case MouseMode::SelectionMode:
        {
            removeHighlightSelectedRect();

            //Move Event nur auslösen, wenn Objekt an neuer GridPosition
            if ((_selectedComponentToMove != nullptr && _selectedComponentToMove->getPosition() != gridPosition) ||
                (_selectedDescriptionToMove != nullptr &&
                 QPointF(_selectedDescriptionToMove->getXPos(), _selectedDescriptionToMove->getYPos()) != gridPosition))
            {
                _model->moveComponent(_selectedComponentToMove, _selectedDescriptionToMove, gridPosition);
            }

                //Multiselection der Bauteile
            else if ((_firstPositionMultiselect != gridPosition) &&
                     (!_model->isThereAComponentOrADescription(gridPosition)))
            {
                for (DescriptionField* description : _model->getDescriptions())
                {
                    description->setSelected(false);
                }
                for (Component* component : _model->getComponents())
                {
                    component->setIsSelected(false);
                }
                _lastPositionMultiselect = scenePosition;
                multiselecting();
                delete _multiselectRect;
                _multiselectRect = nullptr;
                //_firstClickedPositionGrid = QPointF(0,0);
            }

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
                foundComponent->setIsSelected(true);
            }
            else if (hasFoundDescription)
            {
                foundDescription->setSelected(true);
            }
            else if (hasFoundConnection)
            {
                foundConnection->set_isSelected(true);
            }
        }
            break;
        case MouseMode::DescriptionMode:
        {
            if (mouseEvent->button() == Qt::LeftButton)
            {
                bool ok;
                DescriptionField* createdDescription = _model->addDescriptionField(gridPosition, false);
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
            if (_connectionStartComponentPort != nullptr)
            {
                gridDisappears();
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
            for (DescriptionField* description : _model->getDescriptions())
            {
                description->setSelected(false);
            }
            for (Component* component : _model->getComponents())
            {
                component->setIsSelected(false);
            }
            bool isComponentAtPosition = _model->isThereAComponentOrADescription(gridPosition);
            if (isComponentAtPosition)
            {
                QApplication::setOverrideCursor(Qt::ClosedHandCursor);
                _selectedComponentToMove = _model->getComponentAtPosition(gridPosition);
                _selectedDescriptionToMove = _model->getDescriptionAtPosition(gridPosition);

                if (_selectedComponentToMove != nullptr)
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
                //_selectedComponentToMove = nullptr;
                //_selectedDescriptionToMove = nullptr;
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
            if (!_model->isThereAComponentOrADescription(gridPosition))
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
            if (!_model->isThereAComponentOrADescription(gridPosition))
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
            if (foundComponentPort != nullptr)
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
                else if(_mouseIsPressed && _connectionStartComponentPort != nullptr && foundComponentPort->getComponent() != _connectionStartComponentPort->getComponent())
                {
                    gridDisappears();
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
            if (_mouseIsPressed && _componentOrDescriptionIsGrabbed)
            {
                highlightRect(scenePosition, _highlightColor);
            }
            if (_mouseIsPressed && _selectedComponentToMove == nullptr && _selectedDescriptionToMove == nullptr)
            {
                _lastPositionMultiselect = scenePosition;
                multiselecting();
            }
        }
            break;
        case DescriptionMode:
        {
            if (!_model->isThereAComponentOrADescription(gridPosition))
            {
                _sampleDescriptionOnMoveEvent = new DescriptionField(gridPosition.x(), gridPosition.y(), 0);
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

        for (Component* component : _model->getComponents())
        {
            if (component->isSelected())
            {
                _model->getComponentAtPosition(gridPosition);
            }
        }

        for (DescriptionField* description : _model->getDescriptions())
        {
            if (description->isSelected())
            {
                _model->getDescriptionAtPosition(gridPosition);
            }
        }
        editNetworkOrDescription();
    }
}

QPointF NetworkView::scenePositionToGrid(QPointF scenePosition)
{
    qreal xPos = scenePosition.toPoint().x() / Defines::gridLength * Defines::gridLength - (Defines::gridLength / 2);
    qreal yPos = scenePosition.toPoint().y() / Defines::gridLength * Defines::gridLength - (Defines::gridLength / 2);
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

    if (nullptr != _sampleDescriptionOnMoveEvent)
    {
        _model->removeItem(_sampleDescriptionOnMoveEvent);
        delete _sampleDescriptionOnMoveEvent;
        _sampleDescriptionOnMoveEvent = nullptr;

        _model->update();
    }
}

void NetworkView::highlightRect(QPointF scenePosition, QColor highlightColor)
{
    QPointF gridPositionOne = scenePositionToGrid(scenePosition);
    int positionX = gridPositionOne.toPoint().x();
    int positionY = gridPositionOne.toPoint().y();

    if (!_model->isThereAComponentOrADescription(gridPositionOne))
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
    for (Component* component : _model->getComponents())
    {
        if (component->isSelected())
        {
            _model->deleteComponent(component);
            _copiedComponent = nullptr;
        }
    }

    for (DescriptionField* description : _model->getDescriptions())
    {
        if (description->isSelected())
        {
            _model->deleteDescription(description);
            _copiedDescription = nullptr;
        }
    }

    for (Connection* connection : _model->getConnections())
    {
        if (connection->isSelected())
        {
            _model->deleteConnection(connection);
        }
    }
    removeHighlightSelectedRect();
}

/*!
 * \brief Bearbeiten des Netzwerkes oder eines Textfeldes.
 *
 *
 */
void NetworkView::editNetworkOrDescription(void)
{
    for (Component* component : _model->getComponents())
    {
        if (component->isSelected())
        {
            EditView* editView = new EditView(component, _model, false, this, _model->getUndoStack());
            editView->show();
        }
    }

    for (DescriptionField* description : _model->getDescriptions())
    {
        if (description->isSelected())
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

void NetworkView::removeHighlightSelectedRect(void)
{
    for (Component* component : _model->getComponents())
    {
        component->setIsSelected(false);
    }

    for (DescriptionField* descriptionfield : _model->getDescriptions())
    {
        descriptionfield->setSelected(false);
    }

    for (Connection* connection : _model->getConnections())
    {
        connection->set_isSelected(false);
    }

    _model->update();
}

void NetworkView::rotateComponent(QPointF gridPosition, QPointF scenePosition)
{
    if (MouseMode::SelectionMode != _mouseMode)
    {
        _isVerticalComponentDefault = !_isVerticalComponentDefault;
        gridDisappears();
        if (!_model->isThereAComponentOrADescription(gridPosition))
        {
            if (MouseMode::PowerSupplyMode == _mouseMode)
            {
                Component* sampleResistor = new PowerSupply(QString("Q"), gridPosition.toPoint().x(),
                                                            gridPosition.toPoint().y(), _isVerticalComponentDefault, 0,
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
    else if (MouseMode::SelectionMode == _mouseMode)
    {
        for (Component* c : _model->getComponents())
        {
            if (c->isSelected())
            {
                _model->turnComponentRight(c);
            }
        }
    }
}

bool NetworkView::lookingForFreeSpaceToDuplicate(int xPos, int yPos, int& xWaytoTheRight)
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

        xWaytoTheRight += Defines::gridLength;
    }
    return created;
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
        for (DescriptionField* c : _model->getDescriptions())
        {
            averageX += c->getXPos();
            averageY += c->getYPos();
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
 * \brief Dupliziert eine Komponente.
 *
 * Die ausgewählte Komponen wird in ein freies Feld nach rechts dupliziert.
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

    for (DescriptionField* description : _model->getDescriptions())
    {
        if (description->isSelected())
        {
            int xWayToTheRight = Defines::gridLength;
            if (lookingForFreeSpaceToDuplicate(description->getXPos(), description->getYPos(), xWayToTheRight))
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
    for (Component* component : _model->getComponents())
    {
        if (component->isSelected())
        {
            _copiedComponent = component;
            _copiedDescription = nullptr;
        }
    }

    for (DescriptionField* description : _model->getDescriptions())
    {
        if (description->isSelected())
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
    if (!_model->isThereAComponentOrADescription(scenePositionToGrid(_lastClickedPosition)))
    {
        if (_copiedComponent != nullptr)
        {

            _model->duplicateComponent(_copiedComponent, _lastClickedPosition.x(), _lastClickedPosition.y());
        }
        else if (_copiedDescription != nullptr)
        {
            _model->duplicateDescription(_copiedDescription, _lastClickedPosition.x(), _lastClickedPosition.y());
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
    if (event->key() == Qt::Key_Escape)
    {
        setMouseMode(NetworkView::MouseMode::SelectionMode);
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        removeHighlightSelectedRect();
        gridDisappears();

        //Alle selected Objekte auf nullptr setzten, sonst ungewolltes Löschen
        for (Component* component : _model->getComponents())
        {
            if (component->isSelected())
            {
                component->setIsSelected(false);
            }
        }

        for (DescriptionField* description : _model->getDescriptions())
        {
            if (description->isSelected())
            {
                description->setSelected(false);
            }
        }

        for (Connection* connection : _model->getConnections())
        {
            if (connection->isSelected())
            {
                connection->set_isSelected(false);
            }
        }
    }
    QGraphicsView::keyPressEvent(event);
}

void NetworkView::multiselecting(void)
{
    int firstClickedXPosition = _firstPositionMultiselect.toPoint().x();
    int firstClickedYPosition = _firstPositionMultiselect.toPoint().y();
    int lastClickedXPosition = _lastPositionMultiselect.toPoint().x();
    int lastClickedYPosition = _lastPositionMultiselect.toPoint().y();

    if (lastClickedXPosition > firstClickedXPosition && lastClickedYPosition > firstClickedXPosition)
    {
        delete _multiselectRect;
        _multiselectRect = nullptr;
        _multiselectRect = _model->addRect(firstClickedXPosition, firstClickedYPosition,
                                           lastClickedXPosition - firstClickedXPosition,
                                           lastClickedYPosition - firstClickedYPosition);
    }
    else if (firstClickedXPosition > lastClickedXPosition && lastClickedYPosition > firstClickedYPosition)
    {
        delete _multiselectRect;
        _multiselectRect = nullptr;
        _multiselectRect = _model->addRect(lastClickedXPosition, firstClickedYPosition,
                                           firstClickedXPosition - lastClickedXPosition,
                                           lastClickedYPosition - firstClickedYPosition);
    }
    else if (lastClickedXPosition > firstClickedXPosition && firstClickedYPosition > lastClickedYPosition)
    {
        delete _multiselectRect;
        _multiselectRect = nullptr;
        _multiselectRect = _model->addRect(firstClickedXPosition, lastClickedYPosition,
                                           lastClickedXPosition - firstClickedXPosition,
                                           firstClickedYPosition - lastClickedYPosition);
    }
    else if (firstClickedXPosition > lastClickedXPosition && firstClickedYPosition > lastClickedYPosition)
    {
        delete _multiselectRect;
        _multiselectRect = nullptr;
        _multiselectRect = _model->addRect(lastClickedXPosition, lastClickedYPosition,
                                           firstClickedXPosition - lastClickedXPosition,
                                           firstClickedYPosition - lastClickedYPosition);
    }

    for (Component* component : _model->getComponents())
    {
        if ((component->getXPosition() >= firstClickedXPosition) && (component->getXPosition() <= lastClickedXPosition)
            && (component->getYPosition() >= firstClickedYPosition) &&
            (component->getYPosition() <= lastClickedYPosition))
        {
            component->setIsSelected(true);
            highlightRect(component->getPosition(), _highlightColor);
        }
        else if ((component->getXPosition() <= firstClickedXPosition) &&
                 (component->getXPosition() >= lastClickedXPosition)
                 && (component->getYPosition() <= firstClickedYPosition) &&
                 (component->getYPosition() >= lastClickedYPosition))
        {
            component->setIsSelected(true);
            highlightRect(component->getPosition(), _highlightColor);
        }
        else if ((component->getXPosition() >= firstClickedXPosition) &&
                 (component->getXPosition() <= lastClickedXPosition)
                 && (component->getYPosition() <= firstClickedYPosition) &&
                 (component->getYPosition() >= lastClickedYPosition))
        {
            component->setIsSelected(true);
            highlightRect(component->getPosition(), _highlightColor);
        }
        else if ((component->getXPosition() <= firstClickedXPosition) &&
                 (component->getXPosition() >= lastClickedXPosition)
                 && (component->getYPosition() >= firstClickedYPosition) &&
                 (component->getYPosition() <= lastClickedYPosition))
        {
            component->setIsSelected(true);
            highlightRect(component->getPosition(), _highlightColor);
        }
        else
        {
            component->setIsSelected(false);
        }
        _model->update();
    }
    for (DescriptionField* description : _model->getDescriptions())
    {
        if ((description->getXPos() >= firstClickedXPosition) && (description->getXPos() <= lastClickedXPosition)
            && (description->getYPos() >= firstClickedYPosition) && (description->getYPos() <= lastClickedYPosition))
        {
            description->setSelected(true);
            highlightRect(description->getPosition(), _highlightColor);
        }
        else if ((description->getXPos() <= firstClickedXPosition) && (description->getXPos() >= lastClickedXPosition)
                 && (description->getYPos() <= firstClickedYPosition) &&
                 (description->getYPos() >= lastClickedYPosition))
        {
            description->setSelected(true);
            highlightRect(description->getPosition(), _highlightColor);
        }
        else if ((description->getXPos() >= firstClickedXPosition) && (description->getXPos() <= lastClickedXPosition)
                 && (description->getYPos() <= firstClickedYPosition) &&
                 (description->getYPos() >= lastClickedYPosition))
        {
            description->setSelected(true);
            highlightRect(description->getPosition(), _highlightColor);
        }
        else if ((description->getXPos() <= firstClickedXPosition) && (description->getXPos() >= lastClickedXPosition)
                 && (description->getYPos() >= firstClickedYPosition) &&
                 (description->getYPos() <= lastClickedYPosition))
        {
            description->setSelected(true);
            highlightRect(description->getPosition(), _highlightColor);
        }
    }
}
