#include "view/editview.h"

#include <QMessageBox>
#include <model/networkgraphics.h>

EditView::EditView(Component* component, NetworkGraphics* model, bool isInitializingWindow, QWidget* parent) :
        QDialog(parent),
        _editViewUi(new Ui::EditView), _model(model)
{
    _component = component;
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

    connect(_editViewUi->buttonMirrorComponent, SIGNAL(released()), this, SLOT(mirrorElement()));
    connect(_editViewUi->horizontalButton, SIGNAL(clicked()), this, SLOT(updateViewOnChangedOrientation()));
    connect(_editViewUi->verticalButton, SIGNAL(clicked()), this, SLOT(updateViewOnChangedOrientation()));

    QString valueDescription = "";
    QString valuePlaceHolder = "";

    if (_component->getComponentType() == Component::ComponentType::Resistor)
    {
        valueDescription = "Widerstandswert [Ohm]:";
        valuePlaceHolder = "Widerstandswert hier eingeben";
    }
    else if (_component->getComponentType() == Component::ComponentType::PowerSupply)
    {
        valueDescription = "Spannung [V]:";
        valuePlaceHolder = "Spannungswert hier eingeben";
    }

    QRegExp regExp("[1-9][0-9]*$");
    QRegExpValidator* validator = new QRegExpValidator(regExp, this);
    _editViewUi->textEditValue->setValidator(validator);
    _editViewUi->textEditValue->setFocus();

    _editViewUi->labelValue->setText(valueDescription);
    _editViewUi->textEditName->setText(_component->getName());
    _editViewUi->textEditValue->setText(QString::number(_component->getValue()));
    _editViewUi->textEditValue->setPlaceholderText(valuePlaceHolder);

    if (_component->isVertical())
    {
        _isVerticalAtStart = true;
        _editViewUi->verticalButton->setChecked(true);
    }
    else
    {
        _isVerticalAtStart = false;
        _editViewUi->horizontalButton->setChecked(true);
    }
}

void EditView::ok(void)
{
    //Wert prüfen
    QString newValueString = _editViewUi->textEditValue->text();
    bool convertSuccsessful = false;
    int newValue = newValueString.toInt(&convertSuccsessful);

    //Werte übernehmen
    if (convertSuccsessful)
    {
        QString newName = _editViewUi->textEditName->text();
        _component->setName(newName);
        _component->setValue(newValue);
        bool isVerticalNew = _editViewUi->verticalButton->isChecked();
        _component->setVertical(isVerticalNew);

        close();
    }
    else
    {
        QMessageBox::about(this, "Fehler", "Ungültiger Wert wurde eingeben.");
    }
}

void EditView::setupInitilizingView(void)
{
    _editViewUi->buttonMirrorComponent->close();
}

void EditView::mirrorElement(void)
{
    _model->mirrorComponent(_component);
    _hasMirroredComponent = !_hasMirroredComponent;
    _model->update();
}

void EditView::updateViewOnChangedOrientation(void)
{
    bool isVerticalCurrent = _editViewUi->verticalButton->isChecked();
    _component->setVertical(isVerticalCurrent);
    _model->update();
}

void EditView::cancel(void)
{
    //Reset changed Settings
    _component->setVertical(_isVerticalAtStart);
    if(_hasMirroredComponent)
    {
        mirrorElement();
    }

    _model->update();
}

