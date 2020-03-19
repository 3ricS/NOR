/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Das MainWindow ist die Oberfläche das Hauptfenster des Programmes dargestellt wird.
 *
 * Auf diesr Oberfläche können funktionen in der Toolbox und in der Menüleiste ausgewählt werden.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsView>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QLocale>
#include <QToolBar>
#include <QListView>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLocale>

#include <model/networkgraphics.h>
#include "view/networkview.h"
#include "view/powerview.h"
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
    ~MainWindow(void);

public slots:
    //Slots die im View etwas aktualisieren (diese werden mit dem Siganl verknüpft welches im Model emitted wird)
    void setSelectionMode(void);
    void setResistorMode(void);
    void setPowerSupplyMode(void);
    void setConnectionMode(void);
    void setDescriptionMode(void);
    void openFile(void);
    void saveFile(void);
    void saveAsFile(void);
    void zoomIn(void);
    void zoomOut(void);
    void setZoom100Percent(void);
    void createNewFile(void);
    void openAboutWindow(void);
    void duplicate(void);
    void copy(void);
    void paste(void);
    void rotate(void);
    void deleteItem(void);
    void editItem(void);
    void undo(void);
    void redo(void);
    void print(void);
    void cut(void);
    void openCurrentVoltageWindow(void);
    void setCurrentButtonVisibility(bool visibility);

    void updateResistanceValue(void);
    void isPowerSupplyAllowed(bool isAllowed);
    void isRedoPossible(bool canRedo);
    void isUndoPossible(bool canUndo);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void createUpperMenu(void);
    void createToolTips(void);
    void setCheckedInCreateMenu(QAction* actualAction);
    void setSelectionOfModeButtons(QPushButton* actualPushed);
    void createListOfModeButtons(void);

//Konstanten
    static constexpr double _maximumZoom = 299;
    static constexpr double _minimumZoom = 11;

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
    QAction* _print = nullptr;
    QAction* _cut = nullptr;

    QList<QAction*> _createActionGroup;
    QList<QPushButton*> _modusButtons;
    QAction* _undo = nullptr;
    QAction* _redo = nullptr;

    double _scalefactor = 1.0;
};

#endif // MAINWINDOW_H
