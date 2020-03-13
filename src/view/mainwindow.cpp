#include "view/mainwindow.h"

MainWindow::MainWindow(NetworkGraphics* model, QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow),
                                                                  _model(model)
{
    setLocale(QLocale::system());

    _ui->setupUi(this);
    setWindowTitle("NOR - Network of Resistance");
    resize(1080, 720);

    //TODO: Dopplung setScene entfernen
    _networkView = _ui->networkView;
    _ui->networkView->setModel(_model);
    _ui->networkView->setScene(_model);
    setSelectionMode();

    //Zuerst Strom und Spannungs Button verstecken
    setCurrentButtonVisibility(false);

    //Menuleisten Action wird erzeugt
    createUpperMenu();

    //Modus Buttons werden zu einer Liste hinzugefügt
    createListOfModeButtons();

    //ToolTips werden erzeugt
    createToolTips();

    // set connections to model
    connect(_ui->Selection, SIGNAL(released()), this, SLOT(setSelectionMode()));
    connect(_ui->Resistor, SIGNAL(released()), this, SLOT(setResistorMode()));
    connect(_ui->PowerSupply, SIGNAL(released()), this, SLOT(setPowerSupplyMode()));
    connect(_ui->Connection, SIGNAL(released()), this, SLOT(setConnectionMode()));
    connect(_ui->DescriptionField, SIGNAL(released()), this, SLOT(setDescriptionMode()));
    connect(_ui->Undo, SIGNAL(released()), this, SLOT(undo()));
    connect(_ui->Redo, SIGNAL(released()), this, SLOT(redo()));
    connect(_ui->CurrentView, SIGNAL(released()), this, SLOT(openCurrentVoltageWindow()));
    connect(_new, SIGNAL(triggered()), this, SLOT(createNewFile()));
    connect(_saveAs, SIGNAL(triggered()), this, SLOT(saveAsFile()));
    connect(_save, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(_open, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(_ui->PlusZoom, SIGNAL(released()), this, SLOT(zoomIn()));
    connect(_ui->MinusZoom, SIGNAL(released()), this, SLOT(zoomOut()));
    connect(_zoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(_zoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(_zoom100Percent, SIGNAL(triggered()), this, SLOT(setZoom100Percent()));
    connect(_about, SIGNAL(triggered()), this, SLOT(openAboutWindow()));
    connect(_duplicate, SIGNAL(triggered()), this, SLOT(duplicate()));
    connect(_copy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(_paste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(_rotateComponent, SIGNAL(triggered()), this, SLOT(rotate()));
    connect(_deleteComponent, SIGNAL(triggered()), this, SLOT(deleteItem()));
    connect(_edit, SIGNAL(triggered()), this, SLOT(editItem()));
    connect(_selectionMode, SIGNAL(triggered()), this, SLOT(setSelectionMode()));
    connect(_powerSupplyMode, SIGNAL(triggered()), this, SLOT(setPowerSupplyMode()));
    connect(_resistorMode, SIGNAL(triggered()), this, SLOT(setResistorMode()));
    connect(_connectionMode, SIGNAL(triggered()), this, SLOT(setConnectionMode()));
    connect(_descriptionMode, SIGNAL(triggered()), this, SLOT(setDescriptionMode()));
    connect(_print, SIGNAL(triggered()), this, SLOT(print()));

    connect(_model, SIGNAL(resistanceValueChanged(void)), this, SLOT(updateResistanceValue(void)));
    connect(_model, SIGNAL(powerSupplyIsAllowed(bool)), this, SLOT(isPowerSupplyAllowed(bool)));
    connect(_model->getUndoStack(), SIGNAL(canRedoChanged(bool)), this, SLOT(isRedoPossible(bool)));
    connect(_model->getUndoStack(), SIGNAL(canUndoChanged(bool)), this, SLOT(isUndoPossible(bool)));
    connect(_model, SIGNAL(currentAndVoltageIsValid(bool)), this, SLOT(setCurrentButtonVisibility(bool)));
}

/*!
* \brief Setzen des Selection-Modus
*/
void MainWindow::setSelectionMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::SelectionMode);

    setSelectionOfModeButtons(_ui->Selection);
    setCheckedInCreateMenu(_selectionMode);
}

/*!
* \brief Setzen des Widerstands-Modus
*/
void MainWindow::setResistorMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::ResistorMode);

    setSelectionOfModeButtons(_ui->Resistor);
    setCheckedInCreateMenu(_resistorMode);
}

/*!
* \brief Setzen des PowerSupply-Modus
*/
void MainWindow::setPowerSupplyMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::PowerSupplyMode);

    setSelectionOfModeButtons(_ui->PowerSupply);
    setCheckedInCreateMenu(_powerSupplyMode);
}

/*!
* \brief Setzen des DiscriptionField-Modus
*/
void MainWindow::setDescriptionMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::DescriptionMode);

    setSelectionOfModeButtons(_ui->DescriptionField);
    setCheckedInCreateMenu(_descriptionMode);
}

/*!
* \brief Setzen des Connection-Modus
*/
void MainWindow::setConnectionMode(void)
{
    _networkView->setMouseMode(NetworkView::MouseMode::ConnectionMode);

    setSelectionOfModeButtons(_ui->Connection);
    setCheckedInCreateMenu(_connectionMode);
}

void MainWindow::saveFile(void)
{
    _model->save();
    setWindowTitle("NOR - Network of Resistance ~ " + _model->getFileName());
}

void MainWindow::openFile(void)
{
    if (_model->getComponents().count() != 0)
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

void MainWindow::createNewFile(void)
{
    NetworkGraphics* _model = new NetworkGraphics();
    MainWindow* window = new MainWindow(_model);
    window->show();
}

void MainWindow::saveAsFile(void)
{
    _model->saveAs();
    setWindowTitle("NOR - Network of Resistance ~ " + _model->getFileName());
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key::Key_Escape)
    {
        setSelectionMode();
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(_model->hasChangedDocument())
    {
        //MessageBox bevor geschlossen wird
        QMessageBox msgBox;
        msgBox.setLocale(QLocale::system());
        msgBox.setText("Möchten Sie ihre Änderungen im Widerstandsnetzwerk sichern?");
        msgBox.setInformativeText("Ihre Änderungen gehen verloren, wenn Sie sie nicht sichern.");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        //Überschreiben der Button Texte
        msgBox.setButtonText(QMessageBox::Save, tr("Sichern ... "));
        msgBox.setButtonText(QMessageBox::Discard, tr("Nicht sichern"));
        msgBox.setButtonText(QMessageBox::Cancel, tr("Abbrechen"));
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret)
        {
            case QMessageBox::Save:
                _model->save();
                break;
            case QMessageBox::Discard:
                event->accept();
                break;
            case QMessageBox::Cancel:
                event->ignore();
                break;
            default:

                break;
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

    _print = new QAction("Drucken");
    _print->setShortcut(QKeySequence::Print);
    _ui->menuDatei->addAction(_print);

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
    _about->setMenuRole(QAction::ApplicationSpecificRole);

    //Bearbeiten Menu
    _edit = new QAction("Eigenschaften");
    _edit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    _ui->menuBearbeiten->addAction(_edit);

    _undo = _model->getUndoStack()->createUndoAction(_model, tr("Rückgängig"));
    _undo->setShortcut(QKeySequence::Undo);
    _ui->menuBearbeiten->addAction(_undo);

    _redo = _model->getUndoStack()->createRedoAction(_model, tr("Wiederholen"));
    _redo->setShortcut(QKeySequence::Redo);
    _ui->menuBearbeiten->addAction(_redo);

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


    //Erstellen Menü
    _selectionMode = new QAction("Auswahl Maus");
    _selectionMode->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    _selectionMode->setCheckable(true);
    _ui->menuErstellen->addAction(_selectionMode);
    _createActionGroup.append(_selectionMode);
    _selectionMode->setChecked(true);

    _powerSupplyMode = new QAction("Spannungsquelle erstellen");
    _powerSupplyMode->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    _powerSupplyMode->setCheckable(true);
    _ui->menuErstellen->addAction(_powerSupplyMode);
    _createActionGroup.append(_powerSupplyMode);

    _resistorMode = new QAction("Widerstand erstellen");
    _resistorMode->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    _resistorMode->setCheckable(true);
    _ui->menuErstellen->addAction(_resistorMode);
    _createActionGroup.append(_resistorMode);

    _connectionMode = new QAction("Verbindung zeichnen");
    _connectionMode->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    _connectionMode->setCheckable(true);
    _ui->menuErstellen->addAction(_connectionMode);
    _createActionGroup.append(_connectionMode);

    _descriptionMode = new QAction("Textfeld erstellen");
    _descriptionMode->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    _descriptionMode->setCheckable(true);
    _ui->menuErstellen->addAction(_descriptionMode);
    _createActionGroup.append(_descriptionMode);
}

/*!
* \brief Erstellen von Tooltips fürs Erstellen Menü
*/
void MainWindow::createToolTips(void)
{
    _ui->PowerSupply->setToolTip("Spannugsquelle einzeichnen");
    _ui->Resistor->setToolTip("Widerstand einzeichnen");
    _ui->Connection->setToolTip("Bauteile verbinden");
    _ui->DescriptionField->setToolTip("Textfeld erstellen");
    _ui->Selection->setToolTip("Auswahl Maus");
}

void MainWindow::setCheckedInCreateMenu(QAction* actualAction)
{
    for (QAction* a : _createActionGroup)
    {
        a->setChecked(a == actualAction);
    }
}

void MainWindow::setSelectionOfModeButtons(QPushButton* actualPushed)
{
    for (QPushButton* pb : _modusButtons)
    {
        pb->setDown(pb == actualPushed);
    }
}

void MainWindow::createListOfModeButtons(void)
{
    _modusButtons.append(_ui->Selection);
    _modusButtons.append(_ui->PowerSupply);
    _modusButtons.append(_ui->Resistor);
    _modusButtons.append(_ui->Connection);
    _modusButtons.append(_ui->DescriptionField);
}

void MainWindow::setCurrentButtonVisibility(bool visibility)
{
    _ui->CurrentView->setHidden(!visibility);
}

/*!
* \brief Erstellen des "Über"-Fensters
*/
void MainWindow::openAboutWindow(void)
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

void MainWindow::duplicate(void)
{
    _networkView->duplicate();
}

void MainWindow::copy(void)
{
    _networkView->copy();
}

void MainWindow::paste(void)
{
    _networkView->paste();
}

void MainWindow::rotate(void)
{
    _networkView->rotateComponentByShortcut();
}

/*!
* \brief Reinzoomen mit +10% Schritten
*/
void MainWindow::zoomIn(void)
{
    if ((_scalefactor * 100) < _maximumZoom)
    {
        double scaleFactor = 1.1;
        _scalefactor += 0.1;
        _ui->PercentZoom->setText(QString::number(_scalefactor * 100) + "%");
        _ui->networkView->scale(scaleFactor, scaleFactor);

        _model->update();
    }
}

/*!
* \brief Rauszoomen mit -10% Schritten
*/
void MainWindow::zoomOut(void)
{
    if ((_scalefactor * 100) > _minimumZoom)
    {
        double scaleFactor = 0.9;
        _scalefactor -= 0.1;
        _ui->PercentZoom->setText(QString::number(_scalefactor * 100) + "%");
        _ui->networkView->scale(scaleFactor, scaleFactor);
        _model->update();
    }
}

/*!
* \brief Zoom auf 100% stellen
*/
void MainWindow::setZoom100Percent(void)
{
    while (_scalefactor != 1.0)
    {
        if (_scalefactor > 1.0)
        {
            zoomOut();
        }
        else if (_scalefactor < 1.0)
        {
            zoomIn();
        }
    }
}

MainWindow::~MainWindow(void)
{
    delete _ui;
}

void MainWindow::updateResistanceValue(void)
{
    _ui->CalculatedValue->setText(QLocale::system().toString(_model->getResistanceValue(), 'f', 2) + " Ω");
}

void MainWindow::isPowerSupplyAllowed(bool isAllowed)
{
    if (isAllowed)
    {
        _ui->PowerSupply->setFlat(false);
        _ui->PowerSupply->setEnabled(true);
    }
    else
    {
        _ui->PowerSupply->setFlat(true);
        _ui->PowerSupply->setEnabled(false);

        setSelectionMode();
    }
}

void MainWindow::isUndoPossible(bool canUndo)
{
    _ui->Undo->setEnabled(canUndo);
}

void MainWindow::isRedoPossible(bool canRedo)
{
    _ui->Redo->setEnabled(canRedo);
}

void MainWindow::deleteItem(void)
{
    _networkView->deleteSelectedItem();
}

void MainWindow::editItem(void)
{
    _networkView->editNetworkOrDescription();
}

void MainWindow::undo(void)
{
    _undo->triggered();
}

void MainWindow::redo(void)
{
    _redo->triggered();
}

void MainWindow::print(void)
{
    _networkView->print();
}

void MainWindow::openCurrentVoltageWindow()
{
    QString information = _model->getVoltageAndCurrentInformation();
    QMessageBox* m = new QMessageBox("Strom und Spannung", information, QMessageBox::NoIcon, QMessageBox::Ok,
                                     QMessageBox::NoButton, QMessageBox::NoButton, this);
    m->resize(700, 500);
    m->show();

}

