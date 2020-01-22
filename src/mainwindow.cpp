#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <model/model.h>
#include <view/networkgraphics.h>
#include <QListView>
#include <QHBoxLayout>

MainWindow::MainWindow(Model* model, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), _model(model)
{
    ui->setupUi(this);
    this->setWindowTitle("NOR - Network of Resistance");
    this->resize(1800, 1000);

    // setup ui
    _networkScene = new NetworkGraphics();
    ui->networkView->setScene(_networkScene);


    // set connections
    connect(ui->drawButton, SIGNAL(released()), this, SLOT(dropped()));
    connect(_model, SIGNAL(newResistorElement()), this, SLOT(paintSampleLine()));
    connect(_model, SIGNAL(newResistorElement()), this, SLOT(paintSampleResistor()));
}

void MainWindow::dropped()
{
    _model->paintObject();
}

void MainWindow::paintSampleLine()
{
    _networkScene->addConnection(_networkScene, 1630, 120, 30, 800);
    ui->networkView->setScene(_networkScene);
}

void MainWindow::paintSampleResistor()
{
    _networkScene->addResistor(_networkScene, 0, 800);
    _networkScene->addResistor(_networkScene, 1600, 0);
    ui->networkView->setScene(_networkScene);
}

MainWindow::~MainWindow()
{
    delete ui;
}
