#include "view/mainwindow.h"
#include <model/model.h>

#include <QApplication>

int Component::_count = 0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Model* model = new Model();
    MainWindow window(model);
    window.show();

    return a.exec();
}
