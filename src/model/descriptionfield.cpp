#include "descriptionfield.h"

DescriptionField::DescriptionField(int x, int y, int id, QString text)
    : QGraphicsItem(nullptr),
    _xPosition(x), _yPosition(y), _id(id)
{
    if(text != "")
    {
        _text = text;
    }
}

void DescriptionField::setPosition(QPointF gridPosition)
{
    _xPosition = gridPosition.x();
    _yPosition = gridPosition.y();
}

QRectF DescriptionField::boundingRect() const
{
    return QRectF (_xPosition - 100, _yPosition - 50, 150, 100);
}

void DescriptionField::paint(QPainter *painter,[[maybe_unused]] const QStyleOptionGraphicsItem *option, [[maybe_unused]] QWidget *widget)
{
    QFont q;
    q.setPixelSize(13);
    painter->setFont(q);
    painter->drawText(QRectF (_xPosition - 40, _yPosition - 40, 80, 80), _text);

    if(_isSelected)
    {
        paintHighlightRect(painter);
    }
}

/*!
 * \brief Highlightet das Textfeld in einer anderen Farbe.
 *
 * \param[in]   painter
 *
 * Färbt das Textfeld an der ausgewählten Position, mit der eingestelten Farbe, ein.
 */
void DescriptionField::paintHighlightRect(QPainter *painter)
{
    QBrush brush;
    brush.setColor(QColor(255, 0, 0, 55));
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    painter->setBrush(brush);
    painter->drawRect(_xPosition - 50, _yPosition -50, 100, 100);
}

void DescriptionField::set_isSelected(bool isSelected)
{
    _isSelected = isSelected;

}
