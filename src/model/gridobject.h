/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein GridObject ist ein Objekt, dass in dem Raster der Zeichenfläche plaziert wird.
 *
 * Ein GridObject besitzt eine Position innerhalb des Gitters und eine Id.
 * Es ist auf der Zeichenfläche immer in einem Feld des Rasters plaziert.
 */
#ifndef NOR_GRIDOBJECT_H
#define NOR_GRIDOBJECT_H

#include "model/graphicsobject.h"

class GridObject : public GraphicsObject
{
public:
    GridObject(QPointF position, int id) : GraphicsObject(nullptr), _position(position), _id(id) {}
    QRectF boundingRect(void) const override;

    void setPosition(QPointF position) {_position = position;}

    QPointF getPosition(void) {return _position;}
    int getId(void) {return _id;}

protected:
    void paintSelectionHighlight(QPainter* painter) override;

    QPointF     _position;

private:
    const int   _id;

};


#endif //NOR_GRIDOBJECT_H
