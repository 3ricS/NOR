#include "view/mainwindow.h"

MainWindow::MainWindow(NetworkGraphics* model, QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow),
    _model(model)
{
    _ui->setupUi(this);
    setWindowTitle("NOR - Network of Resistance");
    resize(1080, 720);

    // setup ui
    _ui->CalculatedValue->hide();
    _ui->CalculateLabel->hide();

    //TODO: Dopplung setScene entfernen
    _networkView = _ui->networkView;
    _ui->networkView->setModel(_model);
    _ui->networkView->setScene(_model);

    //Menuleisten Action wird erzeugt
    createUpperMenu();

    // set connections to model
    connect(_ui->Selection, SIGNAL(released()), this, SLOT(setSelectionMode()));
    connect(_ui->Resistor, SIGNAL(released()), this, SLOT(setResistorMode()));
    connect(_ui->PowerSupply, SIGNAL(released()), this, SLOT(setPowerSupplyMode()));
    connect(_ui->Connection, SIGNAL(released()), this, SLOT(setConnectionMode()));
    connect(_ui->Calculate, SIGNAL(released()), this, SLOT(setCalculation()));
    connect(_new, SIGNAL(triggered()), this, SLOT(setNewFile()));
    connect(_saveAs, SIGNAL(triggered()), this, SLOT(setSaveAsFile()));
    connect(_save, SIGNAL(triggered()), this, SLOT(setSaveFile()));
    connect(_open, SIGNAL(triggered()), this, SLOT(setOpenFile()));
    connect(_ui->PlusZoom, SIGNAL(released()), this, SLOT(setZoomIn()));
    connect(_ui->MinusZoom, SIGNAL(released()), this, SLOT(setZoomOut()));
    connect(_zoomIn, SIGNAL(triggered()), this, SLOT(setZoomIn()));
    connect(_zoomOut, SIGNAL(triggered()), this, SLOT(setZoomOut()));
    connect(_zoom100Percent, SIGNAL(triggered()), this, SLOT(setZoom100Percent()));
    connect(_about,SIGNAL(triggered()), this, SLOT(openAboutWindow()));
}

//Setzen des Selection Modes
void MainWindow::setSelectionMode()
{
    _networkView->setMouseMode(NetworkView::MouseMode::SelectionMode);
    _ui->Resistor->setDown(false);
    _ui->Connection->setDown(false);
    _ui->PowerSupply->setDown(false);
    _ui->Selection->setDown(true);
}

// Setzen des Widerstands-Modus
void MainWindow::setResistorMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::ResistorMode);
    _ui->Resistor->setDown(true);
    _ui->Connection->setDown(false);
    _ui->PowerSupply->setDown(false);
    _ui->Selection->setDown(false);
}

// Setzen des PowerSupply-Modus
void MainWindow::setPowerSupplyMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::PowerSupplyMode);
    _ui->Resistor->setDown(false);
    _ui->Connection->setDown(false);
    _ui->PowerSupply->setDown(true);
    _ui->Selection->setDown(false);
}

//Setzen des Connection-Modus
void MainWindow::setConnectionMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::ConnectionMode);
    _ui->Resistor->setDown(false);
    _ui->Connection->setDown(true);
    _ui->PowerSupply->setDown(false);
    _ui->Selection->setDown(false);
}

void MainWindow::setCalculation(void)
{
    _ui->CalculateLabel->setHidden(false);
    _ui->CalculatedValue->setHidden(false);
    _ui->CalculatedValue->setText(QString::number(_model->calculate()) + "Ω");

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
    //ToDo: Momentan werden noch Objekte mit weiterzählenden id's erzeugt die Statik sind, wenn jemand ne Idee hat immer man ändern
    NetworkGraphics* _model = new NetworkGraphics();
    MainWindow* window = new MainWindow(_model);
    window->show();
}

void MainWindow::setSaveAsFile()
{
    _model->saveAs();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    _networkView->EscapeKeyPressed(event);
    if(event->key() == Qt::Key::Key_Escape)
    {
        _ui->Resistor->setDown(false);
        _ui->Connection->setDown(false);
        _ui->PowerSupply->setDown(false);
        _ui->Selection->setDown(false);
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

    _zoomIn = new QAction("Zoom In");
    _zoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
    _zoomIn->setStatusTip("Reinzoomen");
    _ui->menuAnsicht->addAction(_zoomIn);

    _zoomOut = new QAction("Zoom Out");
    _zoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
    _zoomOut->setStatusTip("Rauszoomen");
    _ui->menuAnsicht->addAction(_zoomOut);


    _zoom100Percent = new QAction("Zoom Normal");
    _zoom100Percent->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));
    _ui->menuAnsicht->addAction(_zoom100Percent);

    _about = new QAction("Über");
    _ui->menuExtras->addAction(_about);

}

void MainWindow::openAboutWindow()
{
    QMessageBox::about(this, ("About Application"),
                       ("Das Programm <b>NOR - Network of Resistance</b> berechnet den Gesamtwiderstand von Netzwerken "
                        "<br> "
                        "<br> Dieses Programm wurde von folgenden Personen, im Rahmen von Software Engineering 1, geschrieben:"
                        "<br>"
                        "<br> Eric Schniedermeyer"
                        "<br> Leonel Fransen"
                        "<br> Moritz Fichte"
                        "<br> Sören Köstler"
                        "<br>"
                        "<br> Version: 1.0")
                       );
}

//Zoom in
void MainWindow::setZoomIn()
{
    double scaleFactor = 1.1;
    _scalefactor += 0.1;
    _ui->PercentZoom->setText(QString::number(_scalefactor * 100) + "%");
    _ui->networkView->scale(scaleFactor, scaleFactor);

    _model->update();
}

//Zoom out
void MainWindow::setZoomOut()
{
    double scaleFactor = 0.9;
    _scalefactor -= 0.1;
    _ui->PercentZoom->setText(QString::number(_scalefactor * 100) + "%");
    _ui->networkView->scale(scaleFactor, scaleFactor);
    _model->update();
}

//Zoom 100%
void MainWindow::setZoom100Percent()
{
    while (_scalefactor != 1.0)
    {
        if(_scalefactor > 1.0)
        {
            setZoomOut();
        }
        else if(_scalefactor < 1.0)
        {
            setZoomIn();
        }
    }
}

MainWindow::~MainWindow()
{
    delete _ui;
}

