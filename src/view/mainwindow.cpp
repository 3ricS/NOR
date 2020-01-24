#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <model/model.h>
#include <view/networkgraphics.h>

#include <QToolBar>
#include <QListView>
#include <QHBoxLayout>
#include <QComboBox>

#include <QMenu>



MainWindow::MainWindow(Model* model, QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow), _model(model)
{
    _ui->setupUi(this);
    this->setWindowTitle("NOR - Network of Resistance");
    this->resize(1080, 720);

    // setup ui
    _networkScene = new NetworkGraphics();
    _ui->networkView->setScene(_networkScene);



    //setup upper ToolBar
    setupUpperToolBar();

    // set connections
    connect(_ui->drawButton, SIGNAL(released()), this, SLOT(dropped()));
    connect(_model, SIGNAL(newResistorElement()), this, SLOT(paintLine()));
    connect(_model, SIGNAL(newResistorElement()), this, SLOT(paintResistor()));
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

QAction* MainWindow::createMenuAction(QString text, QKeySequence shortcut)
{
    QAction* action = new QAction(text);
    action->setShortcut(shortcut);
    return action;
}

void MainWindow::dropped()
{
    _model->paintObject();
}

void MainWindow::paintLine()
{
    _networkScene->addConnection(1630, 120, 30, 800);
    _ui->networkView->setScene(_networkScene);
    //Hide DrawButton
    _ui->drawButton->hide();
}

void MainWindow::paintResistor()
{
    _networkScene->addResistor(0, 800);
    _networkScene->addResistor(1600, 0);
    _ui->networkView->setScene(_networkScene);
}

MainWindow::~MainWindow()
{
    delete _ui;
}
