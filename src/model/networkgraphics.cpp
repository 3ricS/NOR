#include "view/networkgraphics.h"


void NetworkGraphics::paintResistor(int x, int y)
{
    //Resistor has length of 120 and width of 60
    this->addRect(x, y + 20, 40, 60);
    this->addLine(x + 20, y + 0, x + 20, y + 20);
    this->addLine(x + 20, y + 80, x + 20, y + 100);
}

void NetworkGraphics::paintResistor90Degree(int x, int y)
{
    this->addRect(x - 30, y - 20, 60, 40);
    this->addLine(x -30 , y + 0, x - 50, y + 0);
    this->addLine(x + 30, y + 0, x + 60, y + 0);
}

void NetworkGraphics::paintConnection(int x_start, int y_start, int x_end, int y_end)
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

void NetworkGraphics::paintPowerSupply(int x, int y)
{
    //Power Supply has length of 120 and width of 60
    this->addEllipse(x - 30, y - 30, 60, 60);
    this->addLine(x, y + 60, x, y - 60);
}

void NetworkGraphics::paintPowerSupply90Degree(int x, int y)
{
    //Power Supply has length of 120 and width of 60
    this->addEllipse(x - 30, y - 30, 60, 60);
    this->addLine(x - 60, y, x + 60, y);
}


//Mouse interaction und Entscheidung je nachdem in welchem Modus man ist
void NetworkGraphics::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //Herausfinden der Position die geklicked wurde
    QPointF position = event->scenePos();

    //Ansprechen des Model("Hier wurde etwas geklickt an der und der Position was muss ich tun")
    _model->clickInterpretation(position);
}




