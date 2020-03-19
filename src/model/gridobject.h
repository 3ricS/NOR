/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein GridObject ist ein Objekt, dass sich in der QGraphicScene zeichnen lassen kann.
 *
 * Ein GridObject besitzt eine Position innerhalb des Gitters und eine Id.
 */
#ifndef NOR_GRIDOBJECT_H
#define NOR_GRIDOBJECT_H


#include <QtWidgets/QGraphicsItem>
#include <QtGui/QScreenOrientationChangeEvent>

class GridObject : public QGraphicsItem
{
public:
    GridObject(QPointF position, int id) : QGraphicsItem(nullptr), _position(position), _id(id) {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override = 0;
    QRectF boundingRect(void) const override = 0;

    void setSelected(bool isSelected) {_isSelected = isSelected;}
    void setPosition(QPointF position) {_position = position;}

    bool isSelected(void) {return _isSelected;}
    QPointF getPosition(void) {return _position;}
    int getId(void) {return _id;}

protected:
    QPointF     _position;
    bool        _isSelected = false;

private:
    const int   _id;

};


#endif //NOR_GRIDOBJECT_H
