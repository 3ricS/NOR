#include "view/mainwindow.h"

#include <model/networkgraphics.h>
#include <QApplication>


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
