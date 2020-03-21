#include "view/editview.h"

#include <QMessageBox>
#include <QLocale>
#include <QDebug>

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

    _editViewUi->buttonBox->button(QDialogButtonBox::Cancel)->setText("Abbrechen");
}

void EditView::setupView(void)
{
    _editViewUi->setupUi(this);
    resize(400, 250);
    setWindowTitle("Eigenschaften");

    connect(_editViewUi->buttonTurnRightComponent, SIGNAL(released()), this, SLOT(turnRight()));
    connect(_editViewUi->buttonTurnLeftComponent, SIGNAL(released()), this, SLOT(turnLeft()));

    QString valueDescription = "";
    QString valuePlaceHolder = "";

    if (_component->getComponentTypeInt() == Component::ComponentType::Resistor)
    {
        valueDescription = "Widerstandswert [Ohm]:";
        valuePlaceHolder = "Widerstandswert hier eingeben";

        _editViewUi->actualVoltageView->setText(QLocale::system().toString(_component->getVoltage(), 'f', 2) + "V");
        _editViewUi->actualCurrentView->setText(QLocale::system().toString(_component->getAmp(), 'f', 2) + "A");
    }
    else if (_component->getComponentTypeInt() == Component::ComponentType::PowerSupply)
    {
        hideVoltageLabels();
        valueDescription = "Spannung [V]:";
        _editViewUi->actualCurrentView->setText(QLocale::system().toString(_component->getAmp(), 'f', 2) + "A");
    }

    QRegExp regExp("(([1-9][0-9]*|0)(\\,[0-9]*[1-9])?)");
    QRegExpValidator* validator = new QRegExpValidator(regExp, this);
    _editViewUi->textEditValue->setValidator(validator);
    _editViewUi->textEditValue->setFocus();

    _editViewUi->labelValue->setText(valueDescription);
    _editViewUi->textEditName->setText(_component->getName());

    double value;
    Resistor* resistor = dynamic_cast<Resistor*>(_component);
    bool isResistor = (nullptr != resistor);
    if (isResistor)
    {
        value = resistor->getResistanceValue();
    }
    else
    {
        value = _component->getVoltage();
    }
    QString displayedValue = QLocale::system().toString(value, 'g', 10);
    displayedValue.replace(".", "");
    _editViewUi->textEditValue->setText(displayedValue);
    _editViewUi->textEditValue->setPlaceholderText(valuePlaceHolder);
}

void EditView::accept()
{
    bool nameIsValid = isNameValid();
    bool valueIsValid = isValueValid();
    //Wert prüfen
    if (nameIsValid && valueIsValid)
    {
        editComponent();
        QDialog::accept();
    }
    else
    {
        if (!nameIsValid)
        {
            QMessageBox::about(this, "Üngültige Eingabe", "Bitte Wert eingeben.");
        }
        else
        {
            QMessageBox::about(this, "Üngültige Eingabe", "Ungültiger Wert wurde eingeben.");
        }
    }
}

void EditView::reject()
{
    //Reset changed Settings
    rotateInOriginalPosition();
    _model->update();
    QDialog::reject();
}

void EditView::setupInitilizingView(void)
{
    _editViewUi->buttonTurnLeftComponent->close();
    _editViewUi->buttonTurnRightComponent->close();

    hideCurrentAndVoltageLabels();
}

void EditView::hideCurrentAndVoltageLabels(void)
{
    _editViewUi->CurrentLabel->close();
    _editViewUi->actualCurrentView->close();
    _editViewUi->voltageLabel->close();
    _editViewUi->actualVoltageView->close();
}

void EditView::hideVoltageLabels(void)
{
    _editViewUi->CurrentLabel->setText("Gesamtstrom:");
    _editViewUi->voltageLabel->close();
    _editViewUi->actualVoltageView->close();
}

void EditView::turnRight(void)
{
    _model->rotateComponentRightWithoutUndo(_component);
    _numberOfRotationsRight++;
    _model->update();
}

void EditView::turnLeft(void)
{
    _model->rotateComponentLeftWithoutUndo(_component);
    _numberOfRotationsLeft++;
    _model->update();
}

void EditView::rotateInOriginalPosition(void)
{
    for (int i = 0; i < _numberOfRotationsRight; i++)
    {
        _model->rotateComponentLeftWithoutUndo(_component);
    }
    for (int i = 0; i < _numberOfRotationsLeft; i++)
    {
        _model->rotateComponentRightWithoutUndo(_component);
    }
    _model->update();
}

bool EditView::hasChangedComponent(QString newName, double newValue)
{
    bool changedOrientation = (0 != _numberOfRotationsRight || 0 != _numberOfRotationsLeft);
    bool changedName = (_component->getName() != newName);
    bool changedValue = true;
    if (Component::ComponentType::PowerSupply == _component->getComponentType())
    {
        changedValue = (_component->getVoltage() != newValue);
    }
    else
    {
        Resistor* resistor = dynamic_cast<Resistor*>(_component);
        if (nullptr != resistor)
        {
            changedValue = (resistor->getResistanceValue() != newValue);
        }
    }
    bool changedObject = changedName || changedValue || changedOrientation;
    return changedObject;
}

bool EditView::isNameValid()
{
    QString newNameString = _editViewUi->textEditName->text();
    return ("" != newNameString);
}

bool EditView::isValueValid()
{
    QString newValueString = _editViewUi->textEditValue->text();
    bool valueIsValid = ("" != newValueString);
    if (valueIsValid)
    {
        valueIsValid = false;
        newValueString.replace(',', '.');
        newValueString.toDouble(&valueIsValid);
    }
    return valueIsValid;
}

void EditView::editComponent(void)
{
    QString newName = _editViewUi->textEditName->text();
    QString newValueString = _editViewUi->textEditValue->text();
    newValueString.replace(',', '.');
    double newValue = newValueString.toDouble();

    bool changedObject = hasChangedComponent(newName, newValue);

    if (!changedObject || _isInitializingWindow)
    {
        rotateInOriginalPosition();
        _model->editComponentWithoutUndo(_component, newName, newValue);
    }
    else
    {
        _model->editComponent(_component, newName, newValue, _orientationAtStart);
    }
}

