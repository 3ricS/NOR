#ifndef EDITINGVIEW_H
#define EDITINGVIEW_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QWidget>

#include <model/resistor.h>
#include <model/powersupply.h>
#include <ui_editview.h>

namespace Ui {
    class EditView;
}

class EditView : public QDialog
{
    Q_OBJECT
public:
    explicit EditView(Component* component, QWidget *parent = nullptr);

signals:

private:
    //Methoden
    void setupView(void);
    virtual void accept() override;

    //Variablen
    Ui::EditView* _editViewUi;
    QVBoxLayout* _vLayout = nullptr;
    QHBoxLayout* _hLayout = nullptr;
    QPushButton* _uebernehmenButton = nullptr;
    QLineEdit* _valueInput = nullptr;
    QLineEdit* _nameInput = nullptr;

    Component* _component = nullptr;
};

#endif // EDITINGVIEW_H
