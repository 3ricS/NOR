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

    // set connections to model
    connect(_ui->Resistor, SIGNAL(released()), this, SLOT(setResistorMode()));
    connect(_ui->PowerSupply, SIGNAL(released()), this, SLOT(setPowerSupplyMode()));
    connect(_ui->Connection, SIGNAL(released()), this, SLOT(setConnectionMode()));
    connect(_ui->Calculate, SIGNAL(released()), this, SLOT(setCalculation()));
    connect(_save, SIGNAL(triggered()), this, SLOT(setSaveFile()));
    connect(_load, SIGNAL(triggered()), this, SLOT(setLoadFile()));
    connect(_ui->ZoomIn, SIGNAL(released()), this, SLOT(setZoomIn()));
    connect(_ui->ZoomOut, SIGNAL(released()), this, SLOT(setZoomOut()));

    // connect signals from model

}

// Setzen des Widerstands-Modus
void MainWindow::setResistorMode(void)
{
    _model->setMode(NetworkGraphics::MouseMode::ResistorMode);
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

// Setzen des PowerSupply-Modus
void MainWindow::setPowerSupplyMode(void)
{
    _model->setMode(NetworkGraphics::MouseMode::PowerSupplyMode);
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

//Setzen des Connection-Modus
void MainWindow::setConnectionMode(void)
{
    _model->setMode(NetworkGraphics::MouseMode::ConnectionMode);
    QApplication::setOverrideCursor(Qt::CrossCursor);
}

void MainWindow::setCalculation(void)
{
    _model->calculate();
}

void MainWindow::setSaveFile(void)
{
    _model->save();
}

void MainWindow::setLoadFile(void)
{
    _model->load();
}

//Wenn ESC gedrückt wird, soll es sofort in den SelectionMode Modus gehen
void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
    {
        _model->setMode(NetworkGraphics::MouseMode::SelectionMode);
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
    }
}

void MainWindow::createUpperMenu(void)
{
    _save = new QAction("Speichern");
    _ui->menuDatei->addAction(_save);

    _load = new QAction("Laden");
    _ui->menuDatei->addAction(_load);
}

//Zoom in
void MainWindow::setZoomIn()
{
    double scaleFactor = 1.1;
    _ui->networkView->scale(scaleFactor, scaleFactor);
    _model->update();
}

//Zoom out
void MainWindow::setZoomOut()
{
    double scaleFactor = 0.9;
    _ui->networkView->scale(scaleFactor, scaleFactor);
    _model->update();
}

MainWindow::~MainWindow()
{
    delete _ui;
}
