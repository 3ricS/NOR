#include "view/mainwindow.h"

#include <model/networkgraphics.h>
#include <QApplication>

int Component::_count = 0;
int Resistor::_resistorCount = 0;
int PowerSupply::_powerSupplyCount = 0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetworkGraphics* _model = new NetworkGraphics();
    MainWindow window(_model);
    window.show();
    QSysInfo s;
    if(s.productType() == "windows")
    {
        a.setWindowIcon(QIcon(":/img/ZFlogo.png"));
    }
    else if(s.productType() == "osx")
    {
        a.setWindowIcon(QIcon(":/img/Zf_Logo.icns"));
    }

    return a.exec();
}
