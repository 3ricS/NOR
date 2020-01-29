#include "networkview.h"

NetworkView::NetworkView(QWidget *parent) :
    QGraphicsView(parent)
{
}

void NetworkView::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "Event Pos networkview"
             << event->pos().x()
             << event->pos().y();
    QPointF scenePoint = mapToScene(event->pos());
    _model->mouseReleaseInterpretation(scenePoint);
}

void NetworkView::mousePressEvent(QMouseEvent *event) {
    QPointF scenePoint = mapToScene(event->pos());
    _model->mousePressInterpretation(scenePoint);
}
