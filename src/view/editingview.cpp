#include "editingview.h"


EditingView::EditingView(Component* component,QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Bearbeitunngsansicht");
    _component = component;
    setupView();

}

void EditingView::setupView()
{
    //Größe wird auf 300x300 eingestellt
    resize(300,300);
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
}
