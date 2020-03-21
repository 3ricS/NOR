#include <QtGui/QBrush>
#include <QPainter>

#include "gridobject.h"
#include "defines.h"

/*!
 * \brief Highlightet das Textfeld in einer anderen Farbe.
 *
 * \param painter
 *
 * Färbt das Textfeld an der ausgewählten Position, mit der eingestelten Farbe, ein.
 */
void GridObject::paintSelectionHighlight(QPainter* painter)
{
    QBrush brush;
    brush.setColor(QColor(255, 0, 0, 55));
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    painter->setBrush(brush);
    int xPosition = _position.x();
    int yPosition = _position.y();
    double relactionHighlightToGridSize = 0.93;
    painter->drawRect(xPosition - (Defines::gridLength / 2 * relactionHighlightToGridSize), yPosition - (Defines::gridLength / 2 * relactionHighlightToGridSize),
                      Defines::gridLength * relactionHighlightToGridSize, Defines::gridLength * relactionHighlightToGridSize);
}

/*!
 * \brief Gibt den Bereich an, in dem das GridObject gezeichnet wird.
 *
 * \return Gibt den genutzten Bereich als QRectF zurück.
 */
QRectF GridObject::boundingRect(void) const
{
    int xPosition = _position.x();
    int yPosition = _position.y();
    return QRectF(xPosition - (Defines::gridLength / 2), yPosition - (Defines::gridLength / 2), Defines::gridLength,
                  Defines::gridLength);
}
