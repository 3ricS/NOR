#include "defines.h"
#include "description.h"

Description::Description(int x, int y, int id, QString text)
        : GridObject(QPointF(x, y), id)
{
    if (text != "")
    {
        _text = text;
    }
}

void Description::paint(QPainter* painter, [[maybe_unused]] const QStyleOptionGraphicsItem* option,
                        [[maybe_unused]] QWidget* widget)
{
    QFont q;
    q.setPixelSize(13);
    painter->setFont(q);


    int xPosition = _position.x();
    int yPosition = _position.y();
    painter->drawText(QRectF(xPosition - (Defines::gridLength * 0.4), yPosition - (Defines::gridLength * 0.4),
                             (Defines::gridLength * 0.8), (Defines::gridLength * 0.8)), _text);

    if (_isSelected)
    {
        paintSelectionHighlight(painter);
    }
}