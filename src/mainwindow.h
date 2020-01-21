#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsView>

#include <view/networkgraphics.h>

class Model;

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
    void paintNewResistor();
    void paintNewLine();

private:
    Ui::MainWindow *ui;
    Model* _model = nullptr;
    NetworkGraphics* _scene = nullptr;
    QGraphicsView* _view = nullptr;
};

#endif // MAINWINDOW_H
