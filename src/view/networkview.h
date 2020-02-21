#ifndef NETWORKVIEW_H
#define NETWORKVIEW_H

#include <QMouseEvent>
#include <QGraphicsView>
#include <QDebug>

#include <model/networkgraphics.h>
#include <view/editview.h>

class NetworkView : public QGraphicsView
{
public:
    enum MouseMode{ResistorMode, PowerSupplyMode, ConnectionMode, SelectionMode, DescriptionMode};
    NetworkView(QWidget *parent);
    void setModel(NetworkGraphics* model) {_model = model;}
    void duplicate(void);
    void copy(void);
    void paste(void);
    void rotateComponentByShortcut(void);

    void setMouseMode(MouseMode newMode) {_mouseMode = newMode;}
    void deleteSelectedItem(void);
    void EscapeKeyPressed(QKeyEvent* event);
protected:
    void mouseReleaseEvent(QMouseEvent *mouseEvent) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:

    QPointF scenePositionToGrid(QPointF scenePosition);
    void gridDisappears(void);
    void highlightSelectedRect(QPointF gridPosition);
    void highlightRect(QPointF scenePosition, QColor _highlightColor);
    void removeHighlightSelectedRect(void);
    void rotateComponent(QPointF gridPosition, QPointF scenePosition);

protected:
    void leaveEvent(QEvent* event) override;
    void enterEvent(QEvent* event) override;

private:

    NetworkGraphics* _model  = nullptr;

    bool _mouseIsPressed = false;
    bool _componentIsGrabbed = false;

    //TODO: gehört _connectionStarted & ConnectionStartPosition hierher?
    ComponentPort* _connectionStartComponentPort = new ComponentPort(nullptr, Component::Port::null);
    QGraphicsItem* _previousHighlightedRect = nullptr;
    QGraphicsItem* _selectedRect = nullptr;
    Component* _selectedComponent = nullptr;
    //TODO: _selectedComponentToMove durch _selectedComponent ersetzen
    Component* _selectedComponentToMove = nullptr;
    bool _isVerticalComponentDefault = true;
    Component* _sampleComponentOnMoveEvent = nullptr;
    DescriptionField* _sampleDescriptionOnMoveEvent = nullptr;
    Component* _copiedComponent = nullptr;
    QPointF _lastClickedPositionGrid;
    MouseMode _mouseMode = SelectionMode;
    QColor _highlightColor = QColor(136, 136, 136, 55);  //3 mal 136 ist grau und 55 ist die Transparenz
    QPointF _actualMoveScenePosition;
};

#endif // NETWORKVIEW_H
