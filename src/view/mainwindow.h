#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsView>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>

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
    //TODO: warum ist hier überall ein set vor?
    void setSelectionMode(void);
    void setResistorMode(void);
    void setPowerSupplyMode(void);
    void setConnectionMode(void);
    void setDescriptionMode(void);
    void setSaveFile(void);
    void setOpenFile(void);
    void setZoomIn(void);
    void setZoomOut(void);
    void setZoom100Percent(void);
    void setNewFile(void);
    void setSaveAsFile(void);
    void openAboutWindow(void);
    void setDuplicate(void);
    void setCopy(void);
    void setPaste(void);
    void setRotate(void);
    void deleteItem(void);
    void setEdit(void);
    void undo(void);
    void redo(void);

    void updateResistanceValue(void);

protected:
    void keyReleaseEvent(QKeyEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void createUpperMenu(void);
    void createToolTips(void);
    void setCheckedInCreateMenu(QAction* actualAction);
    void setFlatModusButtonRight(QPushButton* actualPushed);
    void createModusQPushButtonList(void);


//Variables
    Ui::MainWindow*     _ui;
    NetworkGraphics*    _model = nullptr;
    NetworkView*        _networkView = nullptr;
    QAction* _save = nullptr;
    QAction* _saveAs = nullptr;
    QAction* _open = nullptr;
    QAction* _new = nullptr;
    QAction* _zoomIn = nullptr;
    QAction* _zoomOut = nullptr;
    QAction* _zoom100Percent = nullptr;
    QAction* _about = nullptr;
    QAction* _duplicate = nullptr;
    QAction* _copy = nullptr;
    QAction* _paste = nullptr;
    QAction* _rotateComponent = nullptr;
    QAction* _deleteComponent = nullptr;
    QAction* _edit = nullptr;
    QAction* _resistorMode = nullptr;
    QAction* _powerSupplyMode = nullptr;
    QAction* _connectionMode = nullptr;
    QAction* _descriptionMode = nullptr;
    QAction* _selectionMode = nullptr;

    QList<QAction*> _createActionGroup;
    QList<QPushButton*> _modusButtons;
    QAction* _undo = nullptr;
    QAction* _redo = nullptr;

    double _scalefactor = 1;
    bool _ctrlIsPressed = false;
};

#endif // MAINWINDOW_H
