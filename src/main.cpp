#include "view/mainwindow.h"

#include <QApplication>

#include <model/networkgraphics.h>
#include <defines.h>

const QColor Defines::shapeColor = QColor(136, 136, 136, 55);  //3 mal 136 ist grau und 55 ist die Transparenz
const QColor Defines::hightlightColor = QColor(0, 130, 255, 60);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetworkGraphics* _model = new NetworkGraphics();
    MainWindow window(_model);
    window.show();

    QSysInfo s;
    if(s.productType() == "windows")
    {
        a.setWindowIcon(QIcon(":/img/resistorLogo.png"));
    }
    else if(s.productType() == "osx")
    {
        a.setWindowIcon(QIcon(":/img/resistorLogo.icns"));
    }

    return a.exec();
}
