#ifndef NETWORKVIEW_H
#define NETWORKVIEW_H

#include <QMouseEvent>

#include <QGraphicsView>
#include <model/networkgraphics.h>

class NetworkView : public QGraphicsView
{
public:
    NetworkView(QWidget *parent);
    void setModel(NetworkGraphics* model) {_model = model;}

private:
   NetworkGraphics* _model  = nullptr;

protected:
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // NETWORKVIEW_H
