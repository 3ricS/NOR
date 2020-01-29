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
    QPoint scenePoint = mapToScene(event->pos()).toPoint();
    _model->clickInterpretation(scenePoint);
}
