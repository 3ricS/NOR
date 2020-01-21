#include "mainwindow.h"
#include <QApplication>
#include <model/model.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Model* model = new Model();
    MainWindow window(model);
    window.show();

    return a.exec();
}
