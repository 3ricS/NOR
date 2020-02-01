#include "view/editview.h"

#include <QMessageBox>

EditView::EditView(Component* component, QWidget *parent):
    QDialog(parent),
    _editViewUi(new Ui::EditView)
{
    _component = component;
    setupView();

}

void EditView::setupView()
{
    _editViewUi->setupUi(this);
    resize(400, 250);
    setWindowTitle("Eigenschaften");

    QString valueDescription = "";
    QString valuePlaceHolder = "";
    QString orientation = "Vertical";

    if(_component->getComponentType() == Component::ComponentType::Resistor)
    {
        valueDescription = "Widerstandswert [Ohm]:";
        valuePlaceHolder = "Widerstandswert hier eingeben";
    }
    else if(_component->getComponentType() == Component::ComponentType::PowerSupply)
    {
        valueDescription = "Spannung [V]:";
        valuePlaceHolder = "Spannungswert hier eingeben";
    }

    if(!_component->isVertical())
    {
        orientation = "Horizontal";
    }

    _editViewUi->labelValue->setText(valueDescription);
    _editViewUi->textEditName->setText(_component->getName());
    _editViewUi->textEditValue->setText(QString::number(_component->getValue()));
    _editViewUi->textEditValue->setPlaceholderText(valuePlaceHolder);

    _orientationBox = new QComboBox(this);
    _orientationBox->addItem("Vertikal");
    _orientationBox->addItem("Horizontal");
    _orientationBox->setCurrentText(orientation);

    _orientationTitle = new QLabel("Ausrichtung:", this);

    _editViewUi->verticalLayout->addWidget(_orientationTitle);
    _editViewUi->verticalLayout->addWidget(_orientationBox);

}

void EditView::accept()
{

    //Wert prüfen
    QString newValueString = _editViewUi->textEditValue->toPlainText();
    bool convertSuccsessful = false;
    int  newValue = newValueString.toInt(&convertSuccsessful);

    //Werte übernehmen
    if(convertSuccsessful)
    {
        // Name setzen
        QString newName = _editViewUi->textEditName->toPlainText();
        _component->setName(newName);

        //Wert setzen
        _component->setValue(newValue);

        //Orientierung setzen
        bool isVerticalNew = false;
        if(_orientationBox->currentText() == "Horizontal")
        {
            isVerticalNew = false;
        }
        _component->setVertical(isVerticalNew);

        close();
    }
    else
    {
        QMessageBox::about(this, "Fehler", "Ungültiger Wert wurde eingeben.");
    }

}
