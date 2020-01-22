#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <model/model.h>
#include <view/networkgraphics.h>

#include <QListView>
#include <QHBoxLayout>
#include <QComboBox>
#include <QToolBar>


MainWindow::MainWindow(Model* model, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), _model(model)
{
    ui->setupUi(this);
    this->setWindowTitle("NOR - Network of Resistance");
    this->resize(1080, 720);

    // setup ui
    _networkScene = new NetworkGraphics();
    ui->networkView->setScene(_networkScene);



    //setup upper ToolBar
    setupUpperToolBar();

    // set connections
    connect(ui->drawButton, SIGNAL(released()), this, SLOT(dropped()));
    connect(_model, SIGNAL(newResistorElement()), this, SLOT(paintSampleLine()));
    connect(_model, SIGNAL(newResistorElement()), this, SLOT(paintSampleResistor()));
}

// Erstellen der Toolbar
void MainWindow::setupUpperToolBar(void)
{
    //Create upper ToolBar
    _upperToolBar = new QToolBar("upper");

    _upperToolBar->setIconSize(QSize(20, 20));
    _upperToolBar->setOrientation(Qt::Horizontal);
    _upperToolBar->setMovable(false);

    //hinzufügen der ToolBar zum MainWindow
    addToolBar(_upperToolBar);

    //Create Dateibutton für obere ToolBar
    _dateibutton = new QToolButton();
    _dateibutton->showMenu();
    _dateibutton->setText("Datei");
    _dateibutton->setSizeIncrement(50,20);

    //Hinzufügen Button zu ToolBar
    _upperToolBar->addWidget(_dateibutton);
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
