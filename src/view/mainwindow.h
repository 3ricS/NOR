#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsView>
#include <QToolButton>
#include <QMenu>
#include <QAction>

#include <QToolBar>
#include <QListView>
#include <QHBoxLayout>
#include <QComboBox>
#include <QDebug>

#include <model/networkgraphics.h>
#include "view/networkview.h"
#include "ui_mainwindow.h"

class Model;
class NetworkGraphics;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(NetworkGraphics* model, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    //Slots die im View etwas aktualisieren (diese werden mit dem Siganl verknüpft welches im Model emitted wird)
    void setResistorMode(void);
    void setPowerSupplyMode(void);
    void setConnectionMode(void);

private:
    void setupUpperToolBar(void);
    void keyPressEvent(QKeyEvent *event) override;





//Variables
    Ui::MainWindow*     _ui;
    NetworkGraphics*    _model = nullptr;
};

#endif // MAINWINDOW_H
