#include "view/editview.h"


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

    QString valueDescription = "";
    QString valuePlaceHolder = "";
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
    _editViewUi->labelValue->setText(valueDescription);
    _editViewUi->textEditName->setText(_component->getName());
    _editViewUi->textEditValue->setText(QString::number(_component->getValue()));
    _editViewUi->textEditValue->setPlaceholderText(valuePlaceHolder);

    /*
    _vLayout = new QVBoxLayout(this);

    //Erstellt LineEdit für Namen
    _nameInput = new QLineEdit(_component->getName(),this);
    _vLayout->addWidget(_nameInput);

    //Erstellt LineEdit für Value
    _valueInput = new QLineEdit(QString::number(_component->getValue()) ,this);
    _vLayout->addWidget(_valueInput);

    _uebernehmenButton = new QPushButton("Hallo", this);
    _vLayout->addWidget(_uebernehmenButton);

    _valueInput->show();
    _nameInput->show();
    _uebernehmenButton->show();
     */
}

void EditView::accept()
{
    // Name setzen
    QString newName = _editViewUi->textEditName->toPlainText();
    _component->setName(newName);

    //Wert setzen
    QString newValueString = _editViewUi->textEditValue->toPlainText();
    bool convertSuccsessful = false;
    int  newValue = newValueString.toInt(&convertSuccsessful);
    if(convertSuccsessful)
    {
        _component->setValue(newValue);
    } else {
        //TODO: Errormeldung Fehleingabe im textEditValue
    }

    close();
}
