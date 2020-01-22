#ifndef RESISTORELEMENT_H
#define RESISTORELEMENT_H

#include <QGraphicsItem>
#include <QGraphicsScene>

class ResistorViewElement : public QGraphicsItem
{
public:
    ResistorViewElement(int value) {_value = value;}
    void show(QGraphicsScene* scene, int x, int y);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int getValue() {return _value;}


protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    // value of the resistance
    int _value;
};

#endif // RESISTORELEMENT_H
