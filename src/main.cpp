#include "application.h"
#include <defines.h>

const QColor Defines::shapeColor = QColor(136, 136, 136, 55);  //3 mal 136 ist grau und 55 ist die Transparenz
const QColor Defines::hightlightColor = QColor(0, 130, 255, 60);

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    return a.exec();
}
