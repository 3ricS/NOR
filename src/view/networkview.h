#ifndef NETWORKVIEW_H
#define NETWORKVIEW_H

#include <QMouseEvent>
#include <QGraphicsView>
#include <QDebug>
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

    void setMouseMode(MouseMode newMode) {_mouseMode = newMode;}
    void deleteSelectedItem(void);
    void editNetworkOrDescription(void);

public slots:
    void focus();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *mouseEvent) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:

    QPointF scenePositionToGrid(QPointF scenePosition);
    void gridDisappears(void);
    void highlightRect(QPointF scenePosition, QColor _highlightColor);
    void removeHighlightSelectedRect(void);
    void rotateComponent(QPointF gridPosition, QPointF scenePosition);
    bool lookingForFreeSpaceToDuplicate(int xPos, int yPos, int& xWaytoTheRight);
    QPointF findScrollPosition(void);

protected:
    void leaveEvent(QEvent* event) override;
    void enterEvent(QEvent* event) override;

private:

    NetworkGraphics* _model  = nullptr;

    bool _mouseIsPressed = false;
    bool _componentOrDescriptionIsGrabbed = false;

    //TODO: gehört _connectionStarted & ConnectionStartPosition hierher?
    QList<Component*>  _tempComponentListForConnections;
    ComponentPort*    _connectionStartComponentPort = new ComponentPort(nullptr, Component::Port::null);
    QGraphicsItem*    _previousHighlightedRect = nullptr;
    //TODO: _selectedComponentToMove durch _selectedComponent ersetzen
    Component*        _selectedComponentToMove = nullptr;
    DescriptionField* _selectedDescriptionToMove = nullptr;
    bool              _isVerticalComponentDefault = true;
    Component*        _sampleComponentOnMoveEvent = nullptr;
    DescriptionField* _sampleDescriptionOnMoveEvent = nullptr;
    Component*        _copiedComponent = nullptr;
    DescriptionField* _copiedDescription = nullptr;
    QPointF           _lastClickedPositionGrid;
    MouseMode         _mouseMode = SelectionMode;
    QColor            _highlightColor = QColor(136, 136, 136, 55);  //3 mal 136 ist grau und 55 ist die Transparenz
    QPointF           _actualMoveScenePosition;
};

#endif // NETWORKVIEW_H
