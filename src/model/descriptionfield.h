#ifndef DESCRIPTIONFIELD_H
#define DESCRIPTIONFIELD_H

#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>


class DescriptionField : public QGraphicsItem
{
public:
    DescriptionField(int x, int y, int id);

    //Getter
    QString getText(void) const {return _text;}

    //Setter
    void setText(const QString text) {_text = text;}

private:

    int _xPosition;
    int _yPosition;
    int _id;
    QRectF* _rect;
    QString _text = "Ich bin ein Descriptionfield";


    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    // QGraphicsItem interface
public:
};

#endif // DESCRIPTIONFIELD_H
