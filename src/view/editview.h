#ifndef EDITINGVIEW_H
#define EDITINGVIEW_H

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QComboBox>

#include <model/resistor.h>
#include <model/powersupply.h>
#include <ui_editview.h>
#include <model/networkgraphics.h>


namespace Ui {
    class EditView;
}

class NetworkGraphics;

class EditView : public QDialog
{
    Q_OBJECT
public:
    explicit EditView(Component* component, NetworkGraphics* model, bool isInitializingWindow, QWidget* parent,
                      QUndoStack* undoStack);

public slots:
    void turnLeft(void);
    void turnRight(void);

    void cancel(void);
    void ok(void);

private:
    //Methoden
    void setupView(void);
    void setupInitilizingView(void);

    //Variablen
    Ui::EditView*          _editViewUi;
    Component*             _component = nullptr;
    NetworkGraphics*       _model = nullptr;
    Component::Orientation _orientationAtStart;
    QUndoStack*            _undoStack;
    bool                   _isInitializingWindow;
};

#endif // EDITINGVIEW_H
