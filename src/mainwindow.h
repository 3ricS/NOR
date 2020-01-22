#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsView>

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
    void dropped(void);         //Übergabewert dann vlt. das Zeichenelemt welches ausgewählt wurde

//Slots die im View etwas aktualisieren (diese werden mit dem Siganl verknüft welches im Model emitted wird)
    void paintSampleResistor();
    void paintSampleLine();

private:
    Ui::MainWindow *ui;
    Model* _model = nullptr;
    NetworkGraphics* _networkScene = nullptr;
};

#endif // MAINWINDOW_H
