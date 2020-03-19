//
// Created by eric on 19.03.20.
//

#ifndef NOR_GRIDOBJECT_H
#define NOR_GRIDOBJECT_H


#include <QtWidgets/QGraphicsItem>
#include <QtGui/QScreenOrientationChangeEvent>

class GridObject : public QGraphicsItem
{
public:
    GridObject(QPointF position, int id) : QGraphicsItem(nullptr), _position(position), _id(id) {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override = 0;
    QRectF boundingRect() const override = 0;

    void setSelected(bool isSelected) {_isSelected = isSelected;}
    void setPosition(QPointF position) {_position = position;}

    bool isSelected() {return _isSelected;}
    QPointF getPosition(void) {return _position;}
    int getId(void) {return _id;}

protected:
    QPointF     _position;
    bool        _isSelected = false;

private:
    const int   _id;

};


#endif //NOR_GRIDOBJECT_H
