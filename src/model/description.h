/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Ein Description ist ein Textfeld um Notizen und Information zum Netzwerk an die Schaltung zu schreiben.
 *
 * Ein Textfeld besitz die Größe einse Gitters, in dem ein ausgewählter Text stehen kann.
 * Ein Description ist ein QGraphicsItem, damit ein Element der QGraphicsScene und kann sich selbst in die QGraphicsScene
 * zeichnen.
 */
#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <QGraphicsItem>
#include <QPainter>

class Description : public QGraphicsItem
{
public:
    Description(int x, int y, int id, QString text = 0);

    QRectF boundingRect(void) const override;
    void paintHighlightRect(QPainter* painter);

    //Getter
    QString getText(void) const {return _text;}
    int getId(void) {return _id;}
    QPointF getPosition(void) {return QPointF(_xPosition, _yPosition);}
    int getXPosition(void) {return _xPosition;}
    int getYPosition(void) {return _yPosition;}
    bool isSelected(void) const { return  _isSelected;}

    //Setter
    void setText(const QString text) {_text = text;}
    void setPosition(QPointF gridPosition);
    void setIsSelected(bool isSelected);

private:

    int _xPosition;
    int _yPosition;
    int _id;
    bool _isSelected = false;
    QString _text = "Hier könnte dein Text stehen!";


    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

};

#endif // DESCRIPTIONFIELD_H
