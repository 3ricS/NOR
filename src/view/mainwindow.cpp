#include "view/mainwindow.h"

MainWindow::MainWindow(NetworkGraphics* model, QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow),
    _model(model)
{
    _ui->setupUi(this);
    setWindowTitle("NOR - Network of Resistance");
    resize(1080, 720);

    //TODO: Dopplung setScene entfernen
    _networkView = _ui->networkView;
    _ui->networkView->setModel(_model);
    _ui->networkView->setScene(_model);
    setSelectionMode();

    //Menuleisten Action wird erzeugt
    createUpperMenu();

    // set connections to model
    connect(_ui->Selection, SIGNAL(released()), this, SLOT(setSelectionMode()));
    connect(_ui->Resistor, SIGNAL(released()), this, SLOT(setResistorMode()));
    connect(_ui->PowerSupply, SIGNAL(released()), this, SLOT(setPowerSupplyMode()));
    connect(_ui->Connection, SIGNAL(released()), this, SLOT(setConnectionMode()));
    connect(_ui->Calculate, SIGNAL(released()), this, SLOT(setCalculation()));
    connect(_ui->TextButton, SIGNAL(released()), this, SLOT(setDescriptionMode()));
    connect(_new, SIGNAL(triggered()), this, SLOT(setNewFile()));
    connect(_saveAs, SIGNAL(triggered()), this, SLOT(setSaveAsFile()));
    connect(_save, SIGNAL(triggered()), this, SLOT(setSaveFile()));
    connect(_open, SIGNAL(triggered()), this, SLOT(setOpenFile()));
    connect(_ui->PlusZoom, SIGNAL(released()), this, SLOT(setZoomIn()));
    connect(_ui->MinusZoom, SIGNAL(released()), this, SLOT(setZoomOut()));
    connect(_zoomIn, SIGNAL(triggered()), this, SLOT(setZoomIn()));
    connect(_zoomOut, SIGNAL(triggered()), this, SLOT(setZoomOut()));
    connect(_zoom100Percent, SIGNAL(triggered()), this, SLOT(setZoom100Percent()));
    connect(_about, SIGNAL(triggered()), this, SLOT(openAboutWindow()));
    connect(_duplicate, SIGNAL(triggered()), this, SLOT(setDuplicate()));
    connect(_copy, SIGNAL(triggered()), this, SLOT(setCopy()));
    connect(_paste, SIGNAL(triggered()), this, SLOT(setPaste()));
    connect(_rotateComponent, SIGNAL(triggered()), this, SLOT(setRotate()));
    connect(_deleteComponent, SIGNAL(triggered()), this, SLOT(deleteItem()));

    connect(_model, SIGNAL(resistanceValueChanged(void)), this, SLOT(updateResistanceValue(void)));
}

/*!
* \brief Setzen des Selection-Modus
*/
void MainWindow::setSelectionMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::SelectionMode);
    _ui->Resistor->setDown(false);
    _ui->Connection->setDown(false);
    _ui->PowerSupply->setDown(false);
    _ui->Selection->setDown(true);
    _ui->TextButton->setDown(false);
}

/*!
* \brief Setzen des Widerstands-Modus
*/
void MainWindow::setResistorMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::ResistorMode);
    _ui->Resistor->setDown(true);
    _ui->Connection->setDown(false);
    _ui->PowerSupply->setDown(false);
    _ui->Selection->setDown(false);
    _ui->TextButton->setDown(false);
}

/*!
* \brief Setzen des PowerSupply-Modus
*/
void MainWindow::setPowerSupplyMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::PowerSupplyMode);
    _ui->Resistor->setDown(false);
    _ui->Connection->setDown(false);
    _ui->PowerSupply->setDown(true);
    _ui->Selection->setDown(false);
    _ui->TextButton->setDown(false);
}

/*!
* \brief Setzen des DiscriptionField-Modus
*/
void MainWindow::setDescriptionMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::DescriptionMode);
    _ui->TextButton->setDown(true);
    _ui->Resistor->setDown(false);
    _ui->Connection->setDown(false);
    _ui->PowerSupply->setDown(false);
    _ui->Selection->setDown(false);
}

/*!
* \brief Setzen des Connection-Modus
*/
void MainWindow::setConnectionMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::ConnectionMode);
    _ui->Resistor->setDown(false);
    _ui->Connection->setDown(true);
    _ui->PowerSupply->setDown(false);
    _ui->Selection->setDown(false);
    _ui->TextButton->setDown(false);
}

void MainWindow::setCalculation(void)
{
    _ui->CalculatedValue->setText(QString::number(_model->calculate()) + "Ω");
}

void MainWindow::setSaveFile(void)
{
    _model->save();
    setWindowTitle("NOR - Network of Resistance ~ " + _model->getFileName());
}

void MainWindow::setOpenFile(void)
{
        if(_model->getComponents().count() != 0)
        {
            NetworkGraphics* model = new NetworkGraphics();
            MainWindow* window = new MainWindow(model);
            window->show();
            model->load();
            window->setWindowTitle("NOR - Network of Resistance ~ " + model->getFileName());
            setWindowTitle("NOR - Network of Resistance ~ " + _model->getFileName());
        }
        else
        {
            _model->load();
            setWindowTitle("NOR - Network of Resistance ~ " + _model->getFileName());
        }
}

void MainWindow::setNewFile(void)
{
    NetworkGraphics* _model = new NetworkGraphics();
    MainWindow* window = new MainWindow(_model);
    window->show();
}

void MainWindow::setSaveAsFile()
{
    _model->saveAs();
    setWindowTitle("NOR - Network of Resistance ~ " + _model->getFileName());
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    _ctrlIsPressed = false;
    if(event->key() == Qt::Key::Key_Escape)
    {
        setSelectionMode();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key::Key_Control)
    {
        _ctrlIsPressed = true;
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    //TODO: Irgendwie mit Steuerung verknüfen
    if(_ctrlIsPressed)
    {
        if(event->delta() > 0)
        {
        setZoomIn();
        }
        if(event->delta() < 0)
        {
        setZoomOut();
        }
    }
}

void MainWindow::createUpperMenu(void)
{
    //Datei Menu
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

    //Ansichts Menu
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

    //About Menu
    _about = new QAction("Über");
    _ui->menuExtras->addAction(_about);

    //Bearbeiten Menu
    _copy = new QAction("Kopieren");
    _copy->setShortcut(QKeySequence::Copy);
    _ui->menuBearbeiten->addAction(_copy);

    _paste = new QAction("Einfügen");
    _paste->setShortcut(QKeySequence::Paste);
    _ui->menuBearbeiten->addAction(_paste);

    _duplicate = new QAction("Duplizieren");
    _duplicate->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    _ui->menuBearbeiten->addAction(_duplicate);

    _rotateComponent = new QAction("Rotieren");
    _rotateComponent->setShortcut(QKeySequence(Qt::CTRL + Qt::Key::Key_R));
    _ui->menuBearbeiten->addAction(_rotateComponent);

    _deleteComponent = new QAction("Entfernen");
    _deleteComponent->setShortcut(QKeySequence(Qt::Key_Delete));
    _ui->menuBearbeiten->addAction(_deleteComponent);
}

void MainWindow::openAboutWindow()
{
    QMessageBox::about(this, ("About Application"),
                       ("Das Programm <b>NOR - Network of Resistance</b> berechnet den Gesamtwiderstand von Netzwerken "
                        "<br> "
                        "<br> Dieses Programm wurde von folgenden Personen im Rahmen von Software Engineering 1 geschrieben:"
                        "<br>"
                        "<br> Eric Schniedermeyer"
                        "<br> Leonel Fransen"
                        "<br> Moritz Fichte"
                        "<br> Sören Köstler"
                        "<br>"
                        "<br> Version: 1.0")
                       );
}

void MainWindow::setDuplicate()
{
    _networkView->duplicate();
}

void MainWindow::setCopy()
{
    _networkView->copy();
}

void MainWindow::setPaste()
{
    _networkView->paste();
}

void MainWindow::setRotate()
{
    _networkView->rotateComponentByShortcut();
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
    //_ui->networkView->showNormal()
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

void MainWindow::updateResistanceValue(void)
{
    _ui->CalculatedValue->setText(QString::number(_model->getResistanceValue()) + "Ω");
}

void MainWindow::deleteItem(void)
{
    _networkView->deleteSelectedItem();
}

