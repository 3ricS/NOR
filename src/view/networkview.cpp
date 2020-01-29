#include "networkview.h"

NetworkView::NetworkView(QWidget *parent) :
    QGraphicsView(parent)
{
}

void NetworkView::mouseReleaseEvent(QMouseEvent *event)
{
    _model->clickInterpretation(event->pos());
}
