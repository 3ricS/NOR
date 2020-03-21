/*!
 * \author Eric Schniedermeyer, Leonel Fransen, Moritz Fichte, Soeren Koestler
 *
 * \brief   Der EditView ist für die Veränderung des dargestellten Netzwerkes zuständig.
 *
 * Der editView bekommt die Komponenten, das Model,
 * ob das Fenster initialisiert wurde und den UndoStack übergeben.
 */
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

    void accept() override;

    void reject() override;

private:
    //Methoden
    void setupView(void);
    void setupInitilizingView(void);
    void hideCurrentAndVoltageLabels(void);
    void hideVoltageLabels(void);
    void rotateInOriginalPosition(void);
    void editComponent(void);

    bool hasChangedComponent(QString newName, double newValue);
    bool isNameValid();
    bool isValueValid();

private:

    //Variablen
    Ui::EditView*          _editViewUi;
    Component*             _component = nullptr;
    NetworkGraphics*       _model = nullptr;
    Component::Orientation _orientationAtStart;
    QUndoStack*            _undoStack;
    int                    _numberOfRotationsRight = 0;
    int                    _numberOfRotationsLeft = 0;
    bool                   _isInitializingWindow;
};

#endif // EDITINGVIEW_H
