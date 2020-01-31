#ifndef NETWORKVIEW_H
#define NETWORKVIEW_H

#include <QMouseEvent>
#include <QGraphicsView>
#include <QDebug>

#include <model/networkgraphics.h>

class NetworkView : public QGraphicsView
{
public:
    NetworkView(QWidget *parent);
    void setModel(NetworkGraphics* model) {_model = model;}

private:
   NetworkGraphics* _model  = nullptr;

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // NETWORKVIEW_H
