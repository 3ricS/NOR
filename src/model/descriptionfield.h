#ifndef DESCRIPTIONFIELD_H
#define DESCRIPTIONFIELD_H

#include <QGraphicsItem>
#include <QPainter>



class DescriptionField : public QGraphicsItem
{
public:
    DescriptionField(int x, int y, int id);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QString text(void) const {return _text;}
    void setText(const QString text) {_text = text;}

private:

    int _xPosition;
    int _yPosition;
    int _id;
    QString _text = "Hallo";


    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
};

#endif // DESCRIPTIONFIELD_H
