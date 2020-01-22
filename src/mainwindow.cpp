#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <model/model.h>
#include <view/networkgraphics.h>
#include <QListView>
#include <QHBoxLayout>
#include <QToolButton>

MainWindow::MainWindow(Model* model, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), _model(model)
{
    ui->setupUi(this);
    this->setWindowTitle("NOR - Network of Resistance");
    this->resize(1080,720);

    // setup ui
    _networkScene = new NetworkGraphics();
    ui->networkView->setScene(_networkScene);

   QToolBar* _upperToolBar = new QToolBar("upper");
    _upperToolBar->setIconSize(QSize(20, 20));
    _upperToolBar->setOrientation(Qt::Horizontal);
    _upperToolBar->setMovable(false);
    addToolBar(_upperToolBar);

    QToolButton* Dateibutton = new QToolButton();
    Dateibutton->showMenu();
    Dateibutton->setText("Datei");
    Dateibutton->setSizeIncrement(50,20);
    _upperToolBar->addWidget(Dateibutton);

    QToolButton* Buildbutton = new QToolButton();
    Buildbutton->showMenu();
    Buildbutton->setText("Erstellen");
    Buildbutton->setSizeIncrement(50,20);
    _upperToolBar->addWidget(Buildbutton);


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
    _networkScene->addConnection(_networkScene, 330, 400, 430, 500);
    ui->networkView->setScene(_networkScene);
    //Hide DrawButton
    ui->drawButton->hide();
}

void MainWindow::paintSampleResistor()
{
    _networkScene->addResistor(_networkScene, 300, 300);
    _networkScene->addResistor(_networkScene, 400, 500);
    ui->networkView->setScene(_networkScene);
}

MainWindow::~MainWindow()
{
    delete ui;
}