#include "view/editview.h"

#include <QMessageBox>
#include <model/networkgraphics.h>

EditView::EditView(Component* component, NetworkGraphics* model, bool isInitializingWindow, QWidget* parent,
                   QUndoStack* undoStack) :
        QDialog(parent, Qt::WindowCloseButtonHint),
        _editViewUi(new Ui::EditView), _model(model), _undoStack(undoStack), _isInitializingWindow(isInitializingWindow)
{
    _component = component;
    _orientationAtStart = component->getOrientation();
    setupView();

    if (isInitializingWindow)
    {
        setupInitilizingView();
    }
}

void EditView::setupView(void)
{
    _editViewUi->setupUi(this);
    resize(400, 250);
    setWindowTitle("Eigenschaften");

    connect(this, SIGNAL(accepted()), this, SLOT(ok()));
    connect(this, SIGNAL(rejected()), this, SLOT(cancel()));

    connect(_editViewUi->buttonTurnRightComponent, SIGNAL(released()), this, SLOT(turnRight()));
    connect(_editViewUi->buttonTurnLeftComponent, SIGNAL(released()), this, SLOT(turnLeft()));

    QString valueDescription = "";
    QString valuePlaceHolder = "";

    if (_component->getComponentTypeInt() == Component::ComponentType::Resistor)
    {
        valueDescription = "Widerstandswert [Ohm]:";
        valuePlaceHolder = "Widerstandswert hier eingeben";
    }
    else if (_component->getComponentTypeInt() == Component::ComponentType::PowerSupply)
    {
        valueDescription = "Spannung [V]:";
    }

    QRegExp regExp("(([1-9][0-9]*)|0).[0-9]*$");
    QRegExpValidator* validator = new QRegExpValidator(regExp, this);
    _editViewUi->textEditValue->setValidator(validator);
    _editViewUi->textEditValue->setFocus();

    _editViewUi->labelValue->setText(valueDescription);
    _editViewUi->textEditName->setText(_component->getName());
    _editViewUi->textEditValue->setText(QString::number(_component->getValue()));
    _editViewUi->textEditValue->setPlaceholderText(valuePlaceHolder);

    if (_component->getComponentTypeInt() == Component::PowerSupply)
    {
        _editViewUi->labelValue->close();
        _editViewUi->textEditValue->close();
    }
}

void EditView::ok(void)
{
    //Wert prüfen
    QString newValueString = _editViewUi->textEditValue->text();
    if ("" != newValueString)
    {
        bool convertSuccsessful = false;
        newValueString.replace(',', '.');
        double newValue = newValueString.toDouble(&convertSuccsessful);

        //Werte übernehmen
        if (convertSuccsessful)
        {
            QString newName = _editViewUi->textEditName->text();
            if(_isInitializingWindow)
            {
                _model->editComponentWithoutUndo(_component, newName, newValue);
            }
            else
            {
                _model->editComponent(_component, newName, newValue, _orientationAtStart);
            }
            close();
        }
        else
        {
            QMessageBox::about(this, "Üngültige Eingabe", "Ungültiger Wert wurde eingeben.");
        }
    }
    else
    {
        QMessageBox::about(this, "Üngültige Eingabe", "Bitte Namen eingeben.");
    }
}

void EditView::setupInitilizingView(void)
{
    _editViewUi->buttonTurnLeftComponent->close();
    _editViewUi->buttonTurnRightComponent->close();
}

void EditView::turnRight(void)
{
    _model->turnComponentRight(_component);
    _model->update();
}

void EditView::cancel(void)
{
    //Reset changed Settings
    _model->setOrientationOfComponent(_component, _orientationAtStart);
    _model->update();
}

void EditView::turnLeft(void)
{
    _model->turnComponentLeft(_component);
    _model->update();
}

