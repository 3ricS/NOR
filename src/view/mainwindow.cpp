#include "view/mainwindow.h"

MainWindow::MainWindow(NetworkGraphics* model, QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow), _model(model)
{
    _ui->setupUi(this);
    setWindowTitle("NOR - Network of Resistance");
    resize(1080, 720);

    // setup ui
    _ui->networkView->setModel(_model);
    _ui->networkView->setScene(_model);

    //Menuleisten Action wird erzeugt
    createUpperMenu();

    //setup upper ToolBar
    setupUpperToolBar();

    // set connections to model
    connect(_ui->Resistor, SIGNAL(released()), this, SLOT(setResistorMode()));
    connect(_ui->PowerSupply, SIGNAL(released()), this, SLOT(setPowerSupplyMode()));
    connect(_ui->Connection, SIGNAL(released()), this, SLOT(setConnectionMode()));
    connect(_ui->Calculate, SIGNAL(released()), this, SLOT(setCalculation()));
    connect(_save, SIGNAL(triggered()), this, SLOT(setSaveFile()));

    // connect signals from model

}

// Erstellen der Toolbar
void MainWindow::setupUpperToolBar(void)
{
/*
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

*/

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
    _toolBar.addWidget(&_fileButton);*/
}

// Setzen des Widerstands-Modus
void MainWindow::setResistorMode()
{
    _model->setMode(NetworkGraphics::MouseMode::ResistorMode);
}

// Setzen des PowerSupply-Modus
void MainWindow::setPowerSupplyMode()
{
    _model->setMode(NetworkGraphics::MouseMode::PowerSupplyMode);
}

//Setzen des Connection-Modus
void MainWindow::setConnectionMode()
{
    _model->setMode(NetworkGraphics::MouseMode::ConnectionMode);
}

void MainWindow::setCalculation()
{
    _model->calculate();
}

void MainWindow::setSaveFile()
{
    _model->save();
}

//Wenn ESC gedrückt wird, soll es sofort in den SelectionMode Modus gehen
void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
    {
        _model->setMode(NetworkGraphics::MouseMode::SelectionMode);
    }
}

void MainWindow::createUpperMenu()
{
    _save = new QAction("Speichern");
    _ui->menuDatei->addAction(_save);
}

MainWindow::~MainWindow()
{
    delete _ui;
}
