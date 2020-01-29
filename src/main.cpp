#include "view/mainwindow.h"

#include <model/networkgraphics.h>
#include <QApplication>

int Component::_count = 0;
int Resistor::_resistorCount = 0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetworkGraphics* _model = new NetworkGraphics();
    MainWindow window(_model);
    window.show();

    return a.exec();
}
