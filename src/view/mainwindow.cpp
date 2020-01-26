#include "view/mainwindow.h"
#include "ui_mainwindow.h"
#include "QKeyEvent"



MainWindow::MainWindow(Model* model, QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow), _model(model)
{
    _ui->setupUi(this);
    this->setWindowTitle("NOR - Network of Resistance");
    this->resize(1080, 720);

    // setup ui
    _networkScene = new NetworkGraphics(_model);
    _ui->networkView->setScene(_networkScene);



    //setup upper ToolBar
    setupUpperToolBar();

    // set connections to model
    connect(_ui->drawButton, SIGNAL(released()), this, SLOT(dropped()));
    connect(_ui->Resistor, SIGNAL(released()), this, SLOT(setResistorMode()));
    connect(_ui->PowerSupply, SIGNAL(released()), this, SLOT(setPowerSupplyMode()));

    // connect signals from model
    connect(_model, SIGNAL(modelChanged()), this, SLOT(paintView()));
    //connect(_model, SIGNAL(newResistorElement()), this, SLOT(paintResistor()));
}

// Erstellen der Toolbar
void MainWindow::setupUpperToolBar(void)
{
    //Create upper ToolBar
    _toolBar = new QToolBar("upper");
    _toolBar->setIconSize(QSize(20, 20));
    _toolBar->setOrientation(Qt::Horizontal);
    _toolBar->setMovable(false);
    //add ToolBar to MainWindow
    addToolBar(_toolBar);

    //Create File Menu
    _fileMenu = new QMenu("Datei");
    _fileMenu->addAction(_newFile);
    //Action New
    _newFile = new QAction("Neu");
    _fileMenu->addAction(_newFile);
    //Action Open
    _openFile = new QAction("Öffnen");
    _fileMenu->addAction(_openFile);
    //Action Save
    _saveFile = new QAction("Speichern");
    _fileMenu->addAction(_saveFile);

    //Create _fileButton
    _fileButton = new QToolButton();
    _fileButton->setText("Datei");
    //_fileButton->setSizeIncrement(50,20);
    _fileButton->setMenu(_fileMenu);
    _fileButton->setPopupMode(QToolButton::InstantPopup);

    //Add File Button to ToolBar
    _toolBar->addWidget(_fileButton);
}

void MainWindow::dropped()
{

}

void MainWindow::paintView()
{
    for (Component* component : _model->getComponentList())
    {
        // if it is a Resistor
        if(component->getComponentType() == 1)
        {
            _networkScene->paintResistor(component->getXStartPosition(), component->getYStartPosition());
            //Test um zu gucken wie die Power Supply gezeichnet wird

        }
        if(component->getComponentType() == 2)
        {
            _networkScene->paintPowerSupply(component->getXPosition(), component->getYPosition());
        }
    }
}

/*
void MainWindow::paintLine(int x_start, int y_start, int x_end, int y_end)
{
    _networkScene->addConnection(x_start, y_start, x_end, y_end);
    _ui->networkView->setScene(_networkScene);

    //Hide DrawButton
    _ui->drawButton->hide();
}

void MainWindow::paintResistor(int x, int y)
{
    _networkScene->addResistor(x, y);
    _ui->networkView->setScene(_networkScene);
}
*/

// Setzen des Widerstands-Modus
void MainWindow::setResistorMode()
{
    _model->setMode(Model::MouseMode::ResistorMode);
}

// Setzen des PowerSupply-Modus
void MainWindow::setPowerSupplyMode()
{
    _model->setMode(Model::MouseMode::PowerSupplyMode);
}

//Maus-Modus wird eingeschaltet, kein neues Zeichen einer Komponente
void MainWindow::setMouseMode()
{
    _model->setMode(Model::MouseMode::Mouse);
}

//Wenn ESC gedrückt wird soll es sofort in den Mouse Modus gehen
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        setMouseMode();
    }
}

MainWindow::~MainWindow()
{
    delete _ui;
}
