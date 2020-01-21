#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <model/model.h>

MainWindow::MainWindow(Model* model, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), _model(model)
{
    ui->setupUi(this);
    this->setWindowTitle("NOR - Network of Resistance");
    this->resize(1800, 1000);

    QPushButton* draw = new QPushButton("Zeichne");

    _scene = new NetworkGraphics();
    _scene->addWidget(draw);

    _view = new QGraphicsView(_scene);

    setCentralWidget(_view);
    connect(draw,SIGNAL(released()),this,SLOT(dropped()));
    connect(_model,SIGNAL(newResistorElement()),this,SLOT(paintNewLine()));
    connect(_model,SIGNAL(newResistorElement()),this,SLOT(paintNewResistor()));
}

void MainWindow::dropped()
{
    _model->paintObject();
}

void MainWindow::paintNewLine()
{
    _scene->addConnection(_scene, 1630, 120, 30, 800);
    _view->setScene(_scene);
}

void MainWindow::paintNewResistor()
{
    _scene->addResistor(_scene, 0, 800);
    _scene->addResistor(_scene, 1600, 0);
    _view->setScene(_scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}
