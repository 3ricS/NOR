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
    NetworkView(QWidget *parent);
    void setModel(NetworkGraphics* model);
    void duplicate(void);
    void copy(void);
    void paste(void);
    void rotateComponentByShortcut(void);
    void print(void);
    void cut(void);

    void setMouseMode(MouseMode newMode);
    void deleteSelectedItem(void);
    void editNetworkOrDescription(void);
    void deselectAllItems(void);

public slots:
    void focus(void);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *mouseEvent) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:

    static QPointF scenePositionToGrid(QPointF scenePosition);
    void gridDisappears(void);
    void highlightRect(QPointF scenePositionOne, QColor _highlightColor);
    void rotateComponent(QPointF gridPosition, QPointF scenePosition);
    bool lookingForFreeSpaceToDuplicate(int xPos, int yPos, int& xWaytoTheRight);
    QPointF findScrollPosition(void);
    void focusForPrint(void);
    void multiselecting(void);
    void changeOverrideCursor(void);
    Component* findSelectedComponent(void);
    DescriptionField* findSelectedDescription(void);

protected:
    void leaveEvent(QEvent* event) override;
    void enterEvent(QEvent* event) override;

private:

    NetworkGraphics* _model  = nullptr;

    bool _mouseIsPressed = false;
    bool _componentOrDescriptionIsGrabbed = false;
    bool _isMoved = false;

    QList<Component*> _tempComponentListForConnections;
    ComponentPort*    _connectionStartComponentPort = new ComponentPort(nullptr, Component::Port::null);
    QGraphicsItem*    _previousHighlightedRect = nullptr;
    QGraphicsItem*    _previousHighlightedPort = nullptr;
    Component*        _selectedComponentToMove = nullptr;
    DescriptionField* _selectedDescriptionToMove = nullptr;
    bool              _isVerticalComponentDefault = true;
    Component*        _sampleComponentOnMoveEvent = nullptr;
    DescriptionField* _sampleDescriptionOnMoveEvent = nullptr;
    Component*        _copiedComponent = nullptr;
    DescriptionField* _copiedDescription = nullptr;
    QPointF           _lastClickedPosition;
    QGraphicsRectItem*_multiselectRect = nullptr;

    QPointF           _firstPositionMultiselect;
    QPointF           _lastPositionMultiselect;

    MouseMode         _mouseMode = SelectionMode;
    const QColor      _highlightColor = QColor(136, 136, 136, 55);  //3 mal 136 ist grau und 55 ist die Transparenz
    QPointF           _actualMoveScenePosition;
};

#endif // NETWORKVIEW_H
