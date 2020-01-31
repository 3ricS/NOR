#include "networkview.h"

NetworkView::NetworkView(QWidget *parent) :
    QGraphicsView(parent)
{
}

void NetworkView::mouseReleaseEvent(QMouseEvent *event)
{
    QPointF scenePoint = mapToScene(event->pos());
    _model->mouseReleaseInterpretation(scenePoint);
}

void NetworkView::mousePressEvent(QMouseEvent *event) {
    QPointF scenePoint = mapToScene(event->pos());
    _model->mousePressInterpretation(scenePoint);
}

void NetworkView::mouseMoveEvent(QMouseEvent *event)
{
    setMouseTracking(true);
    QPointF scenePoint = mapToScene(event->pos());
    _model->mouseMoveInterpretation(scenePoint);
}

void NetworkView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPointF scenePoint = mapToScene(event->pos());
    _model->mouseDoublePressInterpretation(scenePoint);
}

