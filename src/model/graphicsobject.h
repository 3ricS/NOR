/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein GraphicsObject ist ein Zeichenobjekt, das in der Zeichenebene gezeichnet werden kann.
 */
#ifndef NOR_GRAPHICSOBJECT_H
#define NOR_GRAPHICSOBJECT_H


#include <QtWidgets/QGraphicsItem>

class GraphicsObject : public QGraphicsItem
{
public:
    GraphicsObject(QGraphicsItem* parent) : QGraphicsItem(nullptr) {}


    void setSelected(bool isSelected) {_isSelected = isSelected;}

    bool isSelected(void) {return _isSelected;}

protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override = 0;
    virtual QRectF boundingRect(void) const = 0;

    virtual void paintSelectionHighlight(QPainter* painter) = 0;

    bool        _isSelected = false;
};


#endif //NOR_GRAPHICSOBJECT_H
