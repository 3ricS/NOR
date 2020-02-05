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
    connect(_new, SIGNAL(triggered()), this, SLOT(setNewFile()));
    connect(_saveAs, SIGNAL(triggered()), this, SLOT(setSaveAsFile()));
    connect(_save, SIGNAL(triggered()), this, SLOT(setSaveFile()));
    connect(_open, SIGNAL(triggered()), this, SLOT(setOpenFile()));
    connect(_ui->ZoomIn, SIGNAL(released()), this, SLOT(setZoomIn()));
    connect(_ui->ZoomOut, SIGNAL(released()), this, SLOT(setZoomOut()));

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

void MainWindow::setOpenFile(void)
{
    _model->load();
}

void MainWindow::setNewFile(void)
{

}
void MainWindow::setSaveAsFile()
{

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
    _new = new QAction("Neu");
    _new->setShortcut(QKeySequence::New);
    _new->setStatusTip("Neue Datei erstellen");
    _ui->menuDatei->addAction(_new);

    _open = new QAction("Öffnen");
    _open->setShortcut(QKeySequence::Open);
    _open->setStatusTip("Öffne bestehende Datei");
    _ui->menuDatei->addAction(_open);

    _save = new QAction("Speichern");
    _save->setShortcut(QKeySequence::Save);
    _save->setStatusTip("Speichern der Datei");
    _ui->menuDatei->addAction(_save);

    _saveAs = new QAction("Speichern unter...");
    _saveAs->setShortcut(QKeySequence::SaveAs);
    _saveAs->setStatusTip("Speichern unter neuem Dateipfad");
    _ui->menuDatei->addAction(_saveAs);
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
