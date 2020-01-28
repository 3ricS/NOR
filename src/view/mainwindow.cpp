#include "view/mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(Model* model, QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow), _model(model)
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
    connect(_ui->Resistor, SIGNAL(released()), this, SLOT(setResistorMode()));
    connect(_ui->PowerSupply, SIGNAL(released()), this, SLOT(setPowerSupplyMode()));
    connect(_ui->Connection, SIGNAL(released()), this, SLOT(setConnectionMode()));

    // connect signals from model
    connect(_model, SIGNAL(modelChanged()), this, SLOT(paintView()));
    //connect(_model, SIGNAL(newResistorElement()), this, SLOT(paintResistor()));
}

// Erstellen der Toolbar
void MainWindow::setupUpperToolBar(void)
{

    QToolBar           _toolBar ("upper");
    QToolButton        _fileButton;
    QAction            _newFile;
    QMenu              _fileMenu;
    QAction            _openFile;
    QAction            _saveFile;

    //Create upper ToolBar
    _toolBar.setIconSize(QSize(20, 20));
    _toolBar.setOrientation(Qt::Horizontal);
    _toolBar.setMovable(false);
    //add ToolBar to MainWindow
    addToolBar(&_toolBar);

    /*
    //Create File Menu
    _fileMenu = new QMenu("Datei");
    _fileMenu.addAction(&_newFile);
    //Action New
    _newFile = new QAction("Neu");
    _fileMenu.addAction(&_newFile);
    //Action Open
    _openFile = new QAction("Öffnen");
    _fileMenu.addAction(&_openFile);
    //Action Save
    _saveFile = new QAction("Speichern");
    _fileMenu.addAction(&_saveFile);

    //Create _fileButton
    _fileButton = new QToolButton();
    _fileButton.setText("Datei");
    //_fileButton->setSizeIncrement(50,20);
    _fileButton.setMenu(&_fileMenu);
    _fileButton.setPopupMode(QToolButton::InstantPopup);

    //Add File Button to ToolBar
    _toolBar.addWidget(&_fileButton);
    */
}

void MainWindow::paintView()
{
    for (Component* component : _model->getComponentList())
    {
        // if it is a Resistor
        if(component->getComponentType() == 1)
        {
            _networkScene->paintResistor(component->getXStartPosition(), component->getYStartPosition(), component->isVertical());
            //Test um zu gucken wie die Power Supply gezeichnet wird

        }
        if(component->getComponentType() == 2)
        {
            _networkScene->paintPowerSupply(component->getXPosition(), component->getYPosition(), component->isVertical());
        }
        if(component->getComponentType() == 3)
        {
            _networkScene->paintConnection(component->getXPosition(), component->getYPosition(), 100, 100);
        }
    }
}

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

//Setzen des Connection-Modus
void MainWindow::setConnectionMode()
{
    _model->setMode(Model::MouseMode::ConnectionMode);
}

//Maus-Modus wird eingeschaltet, kein neues Zeichen einer Komponente
void MainWindow::setMouseMode()
{
    _model->setMode(Model::MouseMode::Mouse);
}

//Wenn ESC gedrückt wird, soll es sofort in den Mouse Modus gehen
void MainWindow::keyPressEvent(QKeyEvent* event)
{

}

MainWindow::~MainWindow()
{
    delete _ui;
}
