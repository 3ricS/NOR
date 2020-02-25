/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein DescriptionField ist ein Textfeld um Notizen und Information zum Netzwerk an die Schaltung zu schreiben.
 *
 * Ein Textfeld besitz die Größe einse Gitters, in dem ein ausgewählter Text stehen kann.
 */
#ifndef DESCRIPTIONFIELD_H
#define DESCRIPTIONFIELD_H

#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>


class DescriptionField : public QGraphicsItem
{
public:
    DescriptionField(int x, int y, int id, QString text = 0);

    void paintHighlightRect(QPainter* painter);

    //Getter
    QString getText(void) const {return _text;}
    int getId(void) {return _id;}
    int getXPos(void) {return _xPosition;}
    int getYPos(void) {return _yPosition;}
    bool isSelected(void) const { return  _isSelected;}

    //Setter
    void setText(const QString text) {_text = text;}
    void setPosition(QPointF gridPosition);
    void set_isSelected(bool isSelected);

private:

    int _xPosition;
    int _yPosition;
    int _id;
    bool _isSelected = false;
    QString _text = "Hier könnte dein Text stehen!";


    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    // QGraphicsItem interface
public:
};

#endif // DESCRIPTIONFIELD_H
