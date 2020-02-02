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
    _editViewUi->comboBoxOrientation->addItem("Horizontal");
    _editViewUi->comboBoxOrientation->addItem("Vertikal");
    if(_component->isVertical())
    {
        _editViewUi->comboBoxOrientation->setCurrentIndex(1);
    } else {
        _editViewUi->comboBoxOrientation->setCurrentIndex(0);
    }
}

void EditView::accept()
{

    //Wert prüfen
    QString newValueString = _editViewUi->textEditValue->text();
    bool convertSuccsessful = false;
    int  newValue = newValueString.toInt(&convertSuccsessful);

    //Werte übernehmen
    if(convertSuccsessful)
    {
        QString newName = _editViewUi->textEditName->text();
        _component->setName(newName);
        _component->setValue(newValue);
        bool isVerticalNew = !(_editViewUi->comboBoxOrientation->currentText() == "Horizontal");
        _component->setVertical(isVerticalNew);

        close();
    }
    else
    {
        QMessageBox::about(this, "Fehler", "Ungültiger Wert wurde eingeben.");
    }

}
