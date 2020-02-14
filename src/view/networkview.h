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
    enum MouseMode{ResistorMode, PowerSupplyMode, ConnectionMode, SelectionMode};
    NetworkView(QWidget *parent);
    void setModel(NetworkGraphics* model) {_model = model;}

    void setMouseMode(MouseMode newMode) {_mouseMode = newMode;}
    void deleteSelectedItem(void);
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
    void removeHighlightSelectedRect();

protected:
    void leaveEvent(QEvent* event) override;
    void enterEvent(QEvent* event) override;

    void keyReleaseEvent(QKeyEvent* event) override;

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
    MouseMode _mouseMode = SelectionMode;
    QColor _highlightColor = QColor(136, 136, 136, 55);  //3 mal 136 ist grau und 55 ist die Transparenz
};

#endif // NETWORKVIEW_H
