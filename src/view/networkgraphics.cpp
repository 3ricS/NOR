#include "view/networkgraphics.h"


void NetworkGraphics::addResistor(int value, int x, int y)
{
    ResistorViewElement* resistor = new ResistorViewElement(value);
    resistor->show(this, x, y);
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
