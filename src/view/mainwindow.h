#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsView>
#include <QToolButton>
#include <QMenu>
#include <QAction>

class Model;
class NetworkGraphics;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Model* model,QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    //Slots die dem Model etwas mitteilen:
    void dropped(void);         //Übergabewert, vllt. das Zeichenelemt welches ausgewählt wurde

    //Slots die im View etwas aktualisieren (diese werden mit dem Siganl verknüft welches im Model emitted wird)
    void paintResistor();
    void paintLine();

private:
//Methods
    void setupUpperToolBar(void);
    QAction* createMenuAction(QString text, QKeySequence shortcut);




//Variables
    Ui::MainWindow*     _ui;
    Model*              _model = nullptr;
    NetworkGraphics*    _networkScene = nullptr;
    QToolBar*           _toolBar = nullptr;
    QToolButton*        _fileButton = nullptr;
    QAction*            _newFile = nullptr;
    QMenu*              _fileMenu = nullptr;
    QAction*            _openFile = nullptr;
    QAction*            _saveFile = nullptr;
};

#endif // MAINWINDOW_H
