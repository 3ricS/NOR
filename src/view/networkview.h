/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Der NetworkView ist die Oberfläche in der das Netzwerk dargestellt wird.
 *
 * Auf diesr Oberfläche können Komponenten, Verbindungen und Textfelder erzeugt, bearbeitet und verschoben werden.
 */
#ifndef NETWORKVIEW_H
#define NETWORKVIEW_H

#include <QMouseEvent>
#include <QGraphicsView>
#include <QInputDialog>

#include <model/networkgraphics.h>
#include <view/editview.h>

class NetworkView : public QGraphicsView
{
    Q_OBJECT
public:
    enum MouseMode{ResistorMode, PowerSupplyMode, ConnectionMode, SelectionMode, DescriptionMode};
    Component::ComponentType convertMouseModeToComponentType(MouseMode mouseMode);

    NetworkView(QWidget *parent);
    void setModel(NetworkGraphics* model);
    void duplicate(void);
    void copy(void);
    void paste(void);
    void rotateComponentByShortcut(void);
    void print(void);
    void cut(void);

    void selectAll();
    void selectNothing();

    void setMouseMode(MouseMode newMode);
    void deleteSelectedItems(void);
    void editGridObject(void);

    bool isAllowedToChangeMode(void);

public slots:
    void focus(void);

protected:
    void mouseReleaseEvent(QMouseEvent *mouseEvent) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent* event) override;
    void enterEvent(QEvent* event) override;

private:

    void rotateComponent(QPointF scenePosition);
    QPointF findScrollPosition(void);
    void focusForPrint(void);
    void multiselect(QPointF endOfSelectionPosition, bool isEndOfSelection = false);
    void updateOverrideCursor(void);

    QGraphicsItem* deleteGraphicsItem(QGraphicsItem* graphicsItem);

    //for Key/MouseEvents
    void startConnection(QPointF scenePosition);
    void connectionMoveEvent(QPointF scenePosition);
    void startSelectionOrMove(QPointF scenePosition);
    void startMultiSelection(QPointF scenePosition);
    void startMove(QPointF scenePosition);
    bool moveIfNeeded(QPointF scenePosition);

    void addInitialComponentWithMouseMode(QPointF scenePosition, NetworkView::MouseMode mouseMode);
    void addInitialDescription(QPointF scenePosition);
    void buildConnection(QPointF scenePositionOfEnd);
    void deleteConnectionHighlights();

    //Highlighting
    void deleteSampleObjectsAndHighlights(void);
    void showSampleComponent(QPointF scenePosition, Component::ComponentType componentType);
    void showSampleComponent(QPointF scenePosition, MouseMode mouseMode);
    void showSampleDescription(QPointF scenePosition);
    void highlightGrid(QPointF scenePosition, QColor highlightColor);
    void highlightComponentPort(ComponentPort* componentPortToHighlight, QColor highlightColor);;



    NetworkGraphics*         _model  = nullptr;

    bool                     _mouseIsPressed = false;
    ComponentPort*           _connectionStartComponentPort = nullptr;
    QGraphicsItem*           _previousHighlightedRect = nullptr;
    QGraphicsItem*           _previousHighlightedPort = nullptr;
    GridObject*              _selectedObjectToMove = nullptr;
    bool                     _isVerticalComponentDefault = true;
    Component*               _sampleComponent = nullptr;
    Description*             _sampleDescription = nullptr;
    QList<GridObject*>       _copiedObjects;
    QPointF                  _lastClickedPosition;
    QGraphicsRectItem*       _multiselectRect = nullptr;

    QPointF                  _firstPositionMultiselect;

    MouseMode                _mouseMode = SelectionMode;
    QPointF                  _actualMoveScenePosition;
};

#endif // NETWORKVIEW_H
