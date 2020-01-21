#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("NOR - Network of Resistance");
    this->resize(1800, 1000);

    NetworkGraphics* scene = new NetworkGraphics();
    scene->addResistor(scene, 0, 800);
    scene->addResistor(scene, 1600, 0);

    scene->addConnection(scene, 1630, 120, 30, 800);

    QGraphicsView* networkView = new QGraphicsView(scene);
    setCentralWidget(networkView);
}

MainWindow::~MainWindow()
{
    delete ui;
}
