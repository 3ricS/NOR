#include "view/networkgraphics.h"


void NetworkGraphics::addResistor(NetworkGraphics *scene, int x, int y)
{
    //Resistor has length of 120 and width of 60
    scene->addRect(x, y + 20, 60, 80);
    scene->addLine(x + 30, y + 0, x + 30, y + 20);
    scene->addLine(x + 30, y + 100, x + 30, y + 120);
}

void NetworkGraphics::addConnection(NetworkGraphics *scene, int x_start, int y_start, int x_end, int y_end)
{
    //first calculate the coordinates of the middle
    int x_middle = x_start + 0.5 * (x_end - x_start);
    int y_middle = y_start + 0.5 * (y_end - y_start);

    //draw the four lines to make the connection square
    scene->addLine(x_start, y_start, x_start, y_middle);
    scene->addLine(x_start, y_middle, x_middle, y_middle);
    scene->addLine(x_middle, y_middle, x_end, y_middle);
    scene->addLine(x_end, y_middle, x_end, y_end);
}
