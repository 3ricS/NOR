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
    this->resize(1080, 720);

    // setup ui
    _networkScene = new NetworkGraphics();
    ui->networkView->setScene(_networkScene);

    QToolBar* _upperToolBar = new QToolBar("left");
    _upperToolBar->setIconSize(QSize(20, 20));
    _upperToolBar->setOrientation(Qt::Horizontal);
    _upperToolBar->setMovable(false);
    addToolBar(_upperToolBar);

    QToolButton* dateiButton = new QToolButton();
    dateiButton->showMenu();
    dateiButton->setText("Datei");
    dateiButton->setSizeIncrement(50, 20);
    _upperToolBar->addWidget(dateiButton);

    QToolButton* buildButton = new QToolButton();
    buildButton->showMenu();
    buildButton->setText("Erstellen");
    buildButton->setSizeIncrement(50, 20);
    _upperToolBar->addWidget(buildButton);

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
    _networkScene->addConnection(1630, 120, 30, 800);
    ui->networkView->setScene(_networkScene);
    //Hide DrawButton
    ui->drawButton->hide();
}

void MainWindow::paintSampleResistor()
{
    _networkScene->addResistor(1, 0, 800);
    _networkScene->addResistor(1, 1600, 0);
    ui->networkView->setScene(_networkScene);
}

MainWindow::~MainWindow()
{
    delete ui;
}
