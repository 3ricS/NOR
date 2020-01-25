#include "view/networkgraphics.h"


void NetworkGraphics::addResistor(int x, int y)
{
    //Resistor has length of 120 and width of 60
    this->addRect(x, y + 20, 60, 80);
    this->addLine(x + 30, y + 0, x + 30, y + 20);
    this->addLine(x + 30, y + 100, x + 30, y + 120);
}

void NetworkGraphics::addConnection(int x_start, int y_start, int x_end, int y_end)
{
    //first calculate the coordinates of the middle
    int x_middle = x_start + 0.5 * (x_end - x_start);
    int y_middle = y_start + 0.5 * (y_end - y_start);

    //draw the four lines to make the connection square
    this->addLine(x_start, y_start, x_start, y_middle);
    this->addLine(x_start, y_middle, x_middle, y_middle);
    this->addLine(x_middle, y_middle, x_end, y_middle);
    this->addLine(x_end, y_middle, x_end, y_end);
}

void NetworkGraphics::addPowerSupply(int x, int y)
{
    //Power Supply has length of 120 and width of 60
    this->addEllipse(x - 30, y - 30, 60, 60);
    this->addLine(x, y + 60, x, y - 60);
}


//Mouse interaction und Entscheidung je nachdem in welchem Modus man ist
void NetworkGraphics::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF position = event->scenePos();

    _model->clickInterpretation(position);
}

//Wenn ESC gedrückt wird soll es sofort in den Mouse Modus gehen
void NetworkGraphics::keyPressEvent(QKeyEvent *event)
{

}


