#include "defines.h"
#include "description.h"

Description::Description(int x, int y, int id, QString text)
    : QGraphicsItem(nullptr),
    _xPosition(x), _yPosition(y), _id(id)
{
    if(text != "")
    {
        _text = text;
    }
}

void Description::setPosition(QPointF gridPosition)
{
    _xPosition = gridPosition.x();
    _yPosition = gridPosition.y();
}

QRectF Description::boundingRect(void) const
{
    return QRectF (_xPosition - (Defines::gridLength / 2), _yPosition - (Defines::gridLength / 2), Defines::gridLength, Defines::gridLength);
}

void Description::paint(QPainter *painter, [[maybe_unused]] const QStyleOptionGraphicsItem *option, [[maybe_unused]] QWidget *widget)
{
    QFont q;
    q.setPixelSize(13);
    painter->setFont(q);
    painter->drawText(QRectF (_xPosition - (Defines::gridLength * 0.4), _yPosition - (Defines::gridLength * 0.4), (Defines::gridLength * 0.8), (Defines::gridLength * 0.8)), _text);

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
    painter->drawRect(_xPosition - (Defines::gridLength / 2), _yPosition - (Defines::gridLength / 2), Defines::gridLength, Defines::gridLength);
}

void Description::setIsSelected(bool isSelected)
{
    _isSelected = isSelected;
}
