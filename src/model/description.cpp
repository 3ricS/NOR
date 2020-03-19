#include "defines.h"
#include "description.h"

Description::Description(int x, int y, int id, QString text)
    : GridObject(QPointF(x, y), id)
{
    if(text != "")
    {
        _text = text;
    }
}


QRectF Description::boundingRect(void) const
{
    int xPosition = _position.x();
    int yPosition = _position.y();
    return QRectF (xPosition - (Defines::gridLength / 2), yPosition - (Defines::gridLength / 2), Defines::gridLength, Defines::gridLength);
}

void Description::paint(QPainter *painter, [[maybe_unused]] const QStyleOptionGraphicsItem *option, [[maybe_unused]] QWidget *widget)
{
    QFont q;
    q.setPixelSize(13);
    painter->setFont(q);


    int xPosition = _position.x();
    int yPosition = _position.y();
    painter->drawText(QRectF (xPosition - (Defines::gridLength * 0.4), yPosition - (Defines::gridLength * 0.4), (Defines::gridLength * 0.8), (Defines::gridLength * 0.8)), _text);

    if(_isSelected)
    {
        paintHighlightRect(painter);
    }
}

/*!
 * \brief Highlightet das Textfeld in einer anderen Farbe.
 *
 * \param painter
 *
 * Färbt das Textfeld an der ausgewählten Position, mit der eingestelten Farbe, ein.
 */
void Description::paintHighlightRect(QPainter *painter)
{
    QBrush brush;
    brush.setColor(QColor(255, 0, 0, 55));
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    painter->setBrush(brush);
    int xPosition = _position.x();
    int yPosition = _position.y();
    painter->drawRect(xPosition - (Defines::gridLength / 2), yPosition - (Defines::gridLength / 2), Defines::gridLength, Defines::gridLength);
}
